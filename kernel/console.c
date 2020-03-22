// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int,int);

#define ScreenSize (47*80)
#define Terminals 6

static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
static ushort terminals[Terminals][ScreenSize];
static int positions[Terminals];
static int selectedTerminal = 0;


static int panicked = 0;

#define BACKSPACE 0x100
#define SWITCH 0x110

#define CRTPORT 0x3d4

#define INPUT_BUF 128
struct {
	char buf[INPUT_BUF];
	uint r;  // Read index
	uint w;  // Write index
	uint e;  // Edit index
} input;

void setScreen(int terminal)
{
	if(selectedTerminal == terminal)
		return;
	selectedTerminal = terminal;
	int pos = positions[terminal];
	memmove(crt,terminals[terminal], ScreenSize);
	outb(CRTPORT, 14);
	outb(CRTPORT+1, pos>>8);
	outb(CRTPORT, 15);
	outb(CRTPORT+1, pos);
}

static struct {
	struct spinlock lock;
	int locking;
} cons;

static void
printint(int terminal,int xx, int base, int sign)
{
	static char digits[] = "0123456789abcdef";
	char buf[16];
	int i;
	uint x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);

	if(sign)
		buf[i++] = '-';

	while(--i >= 0)
		consputc(terminal, buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
	int i, c, locking;
	uint *argp;
	char *s;

	locking = cons.locking;
	if(locking)
		acquire(&cons.lock);

	if (fmt == 0)
		panic("null fmt");

	argp = (uint*)(void*)(&fmt + 1);
	for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
		if(c != '%'){
			consputc(selectedTerminal,c);
			continue;
		}
		c = fmt[++i] & 0xff;
		if(c == 0)
			break;
		switch(c){
		case 'd':
			printint(selectedTerminal,*argp++, 10, 1);
			break;
		case 'x':
		case 'p':
			printint(selectedTerminal,*argp++, 16, 0);
			break;
		case 's':
			if((s = (char*)*argp++) == 0)
				s = "(null)";
			for(; *s; s++)
				consputc(selectedTerminal,*s);
			break;
		case '%':
			consputc(selectedTerminal,'%');
			break;
		default:
			// Print unknown % sequence to draw attention.
			consputc(selectedTerminal,'%');
			consputc(selectedTerminal,c);
			break;
		}
	}

	if(locking)
		release(&cons.lock);
}

void
panic(char *s)
{
	int i;
	uint pcs[10];

	cli();
	cons.locking = 0;
	// use lapiccpunum so that we can call panic from mycpu()
	cprintf(selectedTerminal,"lapicid %d: panic: ", lapicid());
	cprintf(selectedTerminal,s);
	cprintf(selectedTerminal,"\n");
	getcallerpcs(&s, pcs);
	for(i=0; i<10; i++)
		cprintf(selectedTerminal," %p", pcs[i]);
	panicked = 1;
	for(;;)
		;
}




static void
cgaputc(int terminal,int c)
{

	//struct inode* node = myproc()->cwd;// myproc();//myproc()->cwd; 
	//int terminal = 0;//node->minor -1;
	int pos = positions[terminal];
	
	// Cursor position: col + 80*row.
	//outb(CRTPORT, 14);
	//pos = inb(CRTPORT+1) << 8;
	//outb(CRTPORT, 15);
	//pos |= inb(CRTPORT+1);

	if(c == '\n')
		pos += 80 - pos%80;
	else if(c == BACKSPACE){
		if(pos > 0) --pos;
	} else
	{
		if (terminal == selectedTerminal)
			crt[pos] =  (c &0xff) | 0x0700;
		terminals[terminal][pos++] =  (c &0xff) | 0x0700;//(c&0xff) | 0x0700;  // black on white
	}
	if(pos < 0 || pos > 25*80)
		panic("pos under/overflow");

	if((pos/80) >= 24){  // Scroll up.
		memmove(terminals[terminal], terminals[terminal]+80, sizeof(terminals[terminal][0])*23*80);
		if (terminal == selectedTerminal)
			memmove(crt, crt+80, sizeof(crt[0])*23*80);
		pos -= 80;
		memset(terminals[terminal]+pos, 0, sizeof(terminals[terminal][0])*(24*80 - pos));
		if (terminal == selectedTerminal)
			memmove(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
	}


	terminals[terminal][pos] = ' ' | 0x0700;
	if (terminal == selectedTerminal)
		crt[pos] = ' ' | 0x0700;

	positions[terminal] = pos;
	if (terminal == selectedTerminal)
	{
		outb(CRTPORT, 14);
		outb(CRTPORT+1, pos>>8);
		outb(CRTPORT, 15);
		outb(CRTPORT+1, pos);
	}

}

void
consputc(int terminal,int c)
{
	//struct inode* node = myproc()->cwd;//node->minor -1
	if(panicked){
		cli();
		for(;;)
			;
	}

	if(terminal == selectedTerminal){
		if(c == BACKSPACE){
			uartputc('\b'); uartputc(' '); uartputc('\b');
		} else
			uartputc(c);
	}
	cgaputc(terminal,c);
}



#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
	int c, doprocdump = 0;

	acquire(&cons.lock);
	while((c = getc()) >= 0){
		switch(c){
		case C('P'):  // Process listing.
			// procdump() locks cons.lock indirectly; invoke later
			doprocdump = 1;
			break;
		case C('U'):  // Kill line.
			while(input.e != input.w &&
			      input.buf[(input.e-1) % INPUT_BUF] != '\n'){
				input.e--;
				consputc(selectedTerminal,BACKSPACE);
			}
			break;
		case C('H'): case '\x7f':  // Backspace
			if(input.e != input.w){
				input.e--;
				consputc(selectedTerminal,BACKSPACE);
			}
			break;
		case SWITCH + 0:
			setScreen(0);
			break;
		case SWITCH + 1:
			setScreen(1);
			break;
		case SWITCH + 2:
			setScreen(2);
			break;
		case SWITCH + 3:
			setScreen(3);
			break;
		case SWITCH + 4:
			setScreen(4);
			break;
		case SWITCH + 5:
			setScreen(5);
			break;
		default:
			if(c != 0 && input.e-input.r < INPUT_BUF){
				c = (c == '\r') ? '\n' : c;
				input.buf[input.e++ % INPUT_BUF] = c;
				consputc(selectedTerminal,c);
				if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
					input.w = input.e;
					wakeup(&input.r);
				}
			}
			break;
		}
	}
	release(&cons.lock);
	if(doprocdump) {
		procdump();  // now call procdump() wo. cons.lock held
	}
}

int
consoleread(struct inode *ip, char *dst, int n)
{
	uint target;
	int c;

	// XXX: Ukloniti ovaj deo.
	if (ip->minor != selectedTerminal + 1)
		return 0;

	iunlock(ip);
	target = n;
	acquire(&cons.lock);
	while(n > 0){
		while(input.r == input.w){
			if(myproc()->killed){
				release(&cons.lock);
				ilock(ip);
				return -1;
			}
			sleep(&input.r, &cons.lock);
		}
		c = input.buf[input.r++ % INPUT_BUF];
		if(c == C('D')){  // EOF
			if(n < target){
				// Save ^D for next time, to make sure
				// caller gets a 0-byte result.
				input.r--;
			}
			break;
		}
		*dst++ = c;
		--n;
		if(c == '\n')
			break;
	}
	release(&cons.lock);
	ilock(ip);

	return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
	int i;

	// XXX: Ukloniti ovaj deo.


	iunlock(ip);
	acquire(&cons.lock);
	int terminal = ip->minor-1;
	for(i = 0; i < n; i++)
		consputc(terminal,buf[i] & 0xff);
	
	release(&cons.lock);
	ilock(ip);

	return n;
}

void
consoleinit(void)
{
	initlock(&cons.lock, "console");

	devsw[CONSOLE].write = consolewrite;
	devsw[CONSOLE].read = consoleread;
	cons.locking = 1;
	memset(crt,0, ScreenSize);
	for (int i = 0; i < Terminals; i++)
	{
		positions[i] = 0;
		memset(terminals[i], 0, ScreenSize);
	}
	
	ioapicenable(IRQ_KBD, 0);
}

