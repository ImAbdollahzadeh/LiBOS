#include "../include/EHCI.h"
#include "../include/TIMER.h"
#include "../include/PRINT.h"
#include "../include/MEMORY.h"
#include "../include/PCI.h"
#include "../include/IDT.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ GLOBAL AND STATIC VARIABLES

static unsigned int    only_one_ehci_on_pci = 0;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void __IMOS_EHCI_MemZero(void* mem, unsigned int bytes)
{
    unsigned int        blocks = bytes >> 3;
    unsigned char       remain = bytes &  7;
    unsigned long long* dst    = (unsigned long long*)mem; 
    unsigned char*      ddst   = 0; 
    while(blocks--) { *dst++ = 0ULL; }
    ddst = (unsigned char*)dst;
    while(remain--) { *ddst++ = 0; } 
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkInit(PLINK x)
{
    x->prev = x;
    x->next = x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkAfter(PLINK a, PLINK x)
{
    PLINK p = a;
    PLINK n = a->next;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkBefore(PLINK a, PLINK x)
{
    PLINK p = a->prev;
    PLINK n = a;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkRemove(PLINK x)
{
    PLINK p = x->prev;
    PLINK n = x->next;
    n->prev = p;
    p->next = n;
    x->next = 0;
    x->prev = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkMoveAfter(PLINK a, PLINK x)
{
    PLINK p = x->prev;
    PLINK n = x->next;
    n->prev = p;
    p->next = n;

    p = a;
    n = a->next;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline void LinkMoveBefore(PLINK a, PLINK x)
{
    PLINK p = x->prev;
    PLINK n = x->next;
    n->prev = p;
    p->next = n;

    p = a->prev;
    n = a;
    n->prev = x;
    x->next = n;
    x->prev = p;
    p->next = x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static inline unsigned int ListIsEmpty(PLINK x)
{
    return x->next == x;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define LinkData(link,T,m) \
    (T *)((char *)(link) - (unsigned long)(&(((T*)0)->m)))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define ListForEach(it, list, m) \
    for (it = LinkData((list).next, typeof(*it), m); \
        &it->m != &(list); \
        it = LinkData(it->m.next, typeof(*it), m))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#define ListForEachSafe(it, n, list, m) \
    for (it = LinkData((list).next, typeof(*it), m), \
        n = LinkData(it->m.next, typeof(*it), m); \
        &it->m != &(list); \
        it = n, \
        n = LinkData(n->m.next, typeof(*it), m))

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static PTRANSFER_DESCRIPTOR EhciAllocTD(PEHCI ehci)
{
    PTRANSFER_DESCRIPTOR      end = ehci->tdPool + MAX_TD;
    for (PTRANSFER_DESCRIPTOR td  = ehci->tdPool; td != end; td++)
    {
        if (!td->active)
        {
            printk("EhciAllocTD ^\n", td);
            td->active = 1;
            return td;
        }
    }

    panic("EhciAllocTD failed\n");
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static PQUEUE_HEAD EhciAllocQH(PEHCI ehci)
{
    PQUEUE_HEAD end = ehci->qhPool + MAX_QH;
    for (PQUEUE_HEAD qh = ehci->qhPool; qh != end; qh++)
    {
        if (!qh->active)
        {
            printk("EhciAllocQH ^\n", qh);
            qh->active = 1;
            return qh;
        }
    }

    panic("EhciAllocQH failed\n");
    return 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciFreeTD(PTRANSFER_DESCRIPTOR td)
{
    printk("EhciFreeTD ^\n", td);
    td->active = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciFreeQH(PQUEUE_HEAD qh)
{
    printk("EhciFreeQH ^\n", qh);
    qh->active = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciInsertAsyncQH(PQUEUE_HEAD list, PQUEUE_HEAD qh)
{
    PQUEUE_HEAD end = LinkData(list->qhLink.prev, QUEUE_HEAD, qhLink); 

    qh->qhlp  = (unsigned int)(void*)list | PTR_QH;
    end->qhlp = (unsigned int)(void*)qh  | PTR_QH;

    LinkBefore(&list->qhLink, &qh->qhLink);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciInsertPeriodicQH(PQUEUE_HEAD list, PQUEUE_HEAD qh)
{
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciRemoveQH(PQUEUE_HEAD qh)
{
    PQUEUE_HEAD prev = LinkData(qh->qhLink.prev, QUEUE_HEAD, qhLink);

    prev->qhlp = qh->qhlp;
    LinkRemove(&qh->qhLink);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciPortSet(volatile unsigned int* portReg, unsigned int data)
{
    unsigned int status = *portReg;
    status |= data;
    status &= ~PORT_RWC;
    *portReg = status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciPortClr(volatile unsigned int* portReg, unsigned int data)
{
    unsigned int status = *portReg;
    status &= ~PORT_RWC;
    status &= ~data;
    status |= PORT_RWC & data;
    *portReg = status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciInitTD(PTRANSFER_DESCRIPTOR td, PTRANSFER_DESCRIPTOR prev, unsigned int toggle, unsigned int packetType, unsigned int len, const void* data)
{
    unsigned int i;
	if (prev)
    {
        prev->link   = (unsigned int)(void*)td;
        prev->tdNext = (unsigned int)(void*)td;
    }

    td->link    = PTR_TERMINATE;
    td->altLink = PTR_TERMINATE;
    td->tdNext  = 0;

    td->token = (toggle << TD_TOK_D_SHIFT) | (len << TD_TOK_LEN_SHIFT) | (3 << TD_TOK_CERR_SHIFT) | (packetType << TD_TOK_PID_SHIFT) | TD_TOK_ACTIVE;

    // Data buffer (not necessarily page aligned)
    void* p          = (void*)data;
    td->buffer[0]    = (unsigned int)p;
    unsigned int tmp = (unsigned int)p;
    td->extBuffer[0] = (unsigned int)(((unsigned long long)tmp >> 32));
    p = (void*)((unsigned int)p & ~0xfff);

    // Remaining pages of buffer memory.
    for (i=1; i<4; i++)
    {
        p = (void*)((unsigned int)p + 0x1000);
        td->buffer[i]    = (unsigned int)p;
        tmp = (unsigned int)p;
        td->extBuffer[i] = (unsigned int)(((unsigned long long)tmp >> 32));
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciInitQH(PQUEUE_HEAD          qh,
					   PUSB_TRANSFER        t,
					   PTRANSFER_DESCRIPTOR td,
					   PUSB_DEVICE          parent, 
					   unsigned int         interrupt, 
					   unsigned int         speed, 
					   unsigned int         addr, 
					   unsigned int         endp, 
					   unsigned int         maxSize)
					   
/* TODO: I need to provide the external HUB address as well as port number of capabilities into QH */

{
    qh->transfer = t;

    unsigned int ch   = (maxSize << QH_CH_MPL_SHIFT) | QH_CH_DTC | (speed << QH_CH_EPS_SHIFT) | (endp << QH_CH_ENDP_SHIFT) | addr;
    unsigned int caps = (1 << QH_CAP_MULT_SHIFT);

    if (!interrupt)
        ch |= 5 << QH_CH_NAK_RL_SHIFT;

    if (speed != USB_HIGH_SPEED && parent)
    {
        if (interrupt)
            // split completion mask - complete on frames 2, 3, or 4
            caps |= (0x1c << QH_CAP_SPLIT_C_SHIFT);
        else
            ch |= QH_CH_CONTROL;

        caps |= (parent->port << QH_CAP_PORT_SHIFT) | (parent->addr << QH_CAP_HUB_ADDR_SHIFT);
    }

    if (interrupt)
        // interrupt schedule mask - start on frame 0
        caps |= (0x01 << QH_CAP_INT_SCHED_SHIFT);

    qh->ch       = ch;
    qh->caps     = caps;
    qh->tdHead   = (unsigned int)(void*)td;
    qh->nextLink = (unsigned int)(void*)td;
    qh->token    = 0;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciProcessQH(PEHCI ehci, PQUEUE_HEAD qh)
{
    PUSB_TRANSFER t = qh->transfer;

    if (qh->token & TD_TOK_HALTED)
    {
        t->success  = 0;
        t->complete = 1;
    }
    else if (qh->nextLink & PTR_TERMINATE)
    {
        if (~qh->token & TD_TOK_ACTIVE)
        {
            if (qh->token & TD_TOK_DATABUFFER)
                panic(" Data Buffer Error\n");
            if (qh->token & TD_TOK_BABBLE)
                panic(" Babble Detected\n");
            if (qh->token & TD_TOK_XACT)
                panic(" Transaction Error\n");
            if (qh->token & TD_TOK_MMF)
                panic(" Missed Micro-Frame\n");

            t->success  = 1;
            t->complete = 1;
        }
    }

    if (t->complete)
    {
        // Clear transfer from queue
        qh->transfer = 0;

        // Update endpoint toggle state
        if (t->success && t->endp)
            t->endp->toggle ^= 1;

        // Remove queue from schedule
        EhciRemoveQH(qh);

        // Free transfer descriptors
        PTRANSFER_DESCRIPTOR td = (PTRANSFER_DESCRIPTOR)(void*)qh->tdHead;
        while (td)
        {
            PTRANSFER_DESCRIPTOR next = (PTRANSFER_DESCRIPTOR)(void*)td->tdNext;
            EhciFreeTD(td);
            td = next;
        }

        // Free queue head
        EhciFreeQH(qh);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciWaitForQH(PEHCI ehci, PQUEUE_HEAD qh)
{
    PUSB_TRANSFER t = qh->transfer;

    while (!t->complete)
        EhciProcessQH(ehci, qh);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static unsigned int EhciResetPort(PEHCI ehci, unsigned int port)
{
    volatile unsigned int* reg = &ehci->opRegs->ports[port];

    // Reset the port
    EhciPortSet(reg, PORT_RESET);
    WaitMiliSecond(50);
    EhciPortClr(reg, PORT_RESET);

    // Wait 100ms for port to enable (TODO - what is appropriate length of time?)
    unsigned int status = 0, i;
    for (i=0; i<10; i++)
    {
        // Delay
        WaitMiliSecond(10);

        // Get current status
        status = *reg;

        // Check if device is attached to port
        if (~status & PORT_CONNECTION)
            break;

        // Acknowledge change in status
        if (status & (PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE))
        {
            EhciPortClr(reg, PORT_ENABLE_CHANGE | PORT_CONNECTION_CHANGE);
            continue;
        }

        // Check if device is enabled
        if (status & PORT_ENABLE)
			break;
    }

    return status;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciDevControl(PUSB_DEVICE dev, PUSB_TRANSFER t)
{
    PEHCI               ehci = (PEHCI)dev->hc;
    PUSB_DEVICE_REQUEST req  = t->req;

    // Determine transfer properties
    unsigned int speed   = dev->speed;
    unsigned int addr    = dev->addr;
    unsigned int maxSize = dev->maxPacketSize;
    unsigned int type    = req->type;
    unsigned int len     = req->len;

    // Create queue of transfer descriptors
    PTRANSFER_DESCRIPTOR td = EhciAllocTD(ehci);
    if (!td)
        return;

    PTRANSFER_DESCRIPTOR head = td;
    PTRANSFER_DESCRIPTOR prev = 0;

    // Setup packet
    unsigned int toggle     = 0;
    unsigned int packetType = USB_PACKET_SETUP;
    unsigned int packetSize = sizeof(USB_DEVICE_REQUEST);
    EhciInitTD(td, prev, toggle, packetType, packetSize, req);
    prev = td;

    // Data in/out packets
    packetType = type & RT_DEV_TO_HOST ? USB_PACKET_IN : USB_PACKET_OUT;

    unsigned char* it = (unsigned char*)t->data;
    unsigned char* end = it + len;
    while (it < end)
    {
        td = EhciAllocTD(ehci);
        if (!td)
            return;

        toggle ^= 1;
        packetSize = end - it;
        if (packetSize > maxSize)
            packetSize = maxSize;

        EhciInitTD(td, prev, toggle, packetType, packetSize, it);

        it  += packetSize;
        prev = td;
    }

    // Status packet
    td = EhciAllocTD(ehci);
    if (!td)
        return;

    toggle = 1;
    packetType = type & RT_DEV_TO_HOST ? USB_PACKET_OUT : USB_PACKET_IN;
    EhciInitTD(td, prev, toggle, packetType, 0, 0);

    // Initialize queue head
    PQUEUE_HEAD qh = EhciAllocQH(ehci);
    EhciInitQH(qh, t, head, dev->parent, 0, speed, addr, 0, maxSize);

    // Wait until queue has been processed
    EhciInsertAsyncQH(ehci->asyncQH, qh);
    EhciWaitForQH(ehci, qh);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciDevIntr(PUSB_DEVICE dev, PUSB_TRANSFER t)
{
	return;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int UsbDevRequest(PUSB_DEVICE dev, unsigned int type, unsigned int request, unsigned int value, unsigned int index, unsigned int len, void* data)
{
    USB_DEVICE_REQUEST req;
    req.type  = type;
    req.req   = request;
    req.value = value;
    req.index = index;
    req.len   = len;

    USB_TRANSFER t;
    t.endp     = 0;
    t.req      = &req;
    t.data     = data;
    t.len      = len;
    t.complete = 0;
    t.success  = 0;

    dev->hcControl(dev, &t);

    return t.success;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

unsigned int UsbDevInit(PUSB_DEVICE dev)
{
    USB_DEVICE_DESCRIPTOR devDesc;
	
    if (!UsbDevRequest(dev, RT_DEV_TO_HOST | RT_STANDARD | RT_DEV, REQ_GET_DESC, (USB_DESC_DEVICE << 8) | 0, 0, 8, &devDesc))
        return 0;
	
    return 1;
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciProbe(PEHCI ehci, PUSB_DEVICE dev)
{
    unsigned int port, speed, status;
	// Port setup
    unsigned int portCount = ehci->capRegs->hcsParams & HCSPARAMS_N_PORTS_MASK;
    for (port=0; port<portCount; port++)
    {
        // Reset port
        status = EhciResetPort(ehci, port);

        if (status & PORT_ENABLE)
        {
            speed = USB_HIGH_SPEED;
            if (dev)
            {
                dev->parent        = 0;
                dev->hc            = ehci;
                dev->port          = port;
                dev->speed         = speed;
                dev->maxPacketSize = 8;
                dev->hcControl     = EhciDevControl;
                dev->hcIntr        = EhciDevIntr;

                if (!UsbDevInit(dev))
                    panic("USB Device Initialization failed\n");
            }
        }
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciControllerPollList(PEHCI ehci, PLINK list)
{
    PQUEUE_HEAD qh;
    PQUEUE_HEAD next;
    ListForEachSafe(qh, next, *list, qhLink)
    {
        if (qh->transfer)
            EhciProcessQH(ehci, qh);
    }
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static void EhciControllerPoll(PUSB_CONTROLLER controller)
{
    PEHCI ehci = (PEHCI)controller->hc;

    EhciControllerPollList(ehci, &ehci->asyncQH->qhLink);
    EhciControllerPollList(ehci, &ehci->periodicQH->qhLink);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void EhciBegin(PEHCI ehci)
{
    unsigned int i;
    unsigned int legsup;
    
	USB_DEVICE   usb_device;
	if (sizeof(QUEUE_HEAD) != 108)
    {
        printk("Unexpected QUEUE_HEAD size: %\n", sizeof(QUEUE_HEAD));
        return;
    }

    printk ("Initializing EHCI\n");
    
    // Controller initialization
    ehci->capRegs   = (PCAPABILITY_REGISTERS)(void*)ehci->bar0;
    ehci->opRegs    = (POPERATION_REGISTERS) (void*)((unsigned int)ehci->bar0 + ehci->capRegs->capLength);
    ehci->frameList = (unsigned int*)         Alloc(1024 * sizeof(unsigned int));
    ehci->qhPool    = (PQUEUE_HEAD)           Alloc(sizeof(QUEUE_HEAD) * MAX_QH);
    ehci->tdPool    = (PTRANSFER_DESCRIPTOR)  Alloc(sizeof(TRANSFER_DESCRIPTOR) * MAX_TD);

    __IMOS_EHCI_MemZero(ehci->qhPool, sizeof(QUEUE_HEAD) * MAX_QH);
    __IMOS_EHCI_MemZero(ehci->tdPool, sizeof(TRANSFER_DESCRIPTOR) * MAX_TD);

    // Asynchronous queue setup
    PQUEUE_HEAD qh = EhciAllocQH(ehci);
    qh->qhlp      = (unsigned int)(void*)qh | PTR_QH;
    qh->ch        = QH_CH_H;
    qh->caps      = 0;
    qh->curLink   = 0;
    qh->nextLink  = PTR_TERMINATE;
    qh->altLink   = 0;
    qh->token     = 0;
    for (i=0; i<5; i++)
    {
        qh->buffer[i]    = 0;
        qh->extBuffer[i] = 0;
    }
    qh->transfer    = 0;
    qh->qhLink.prev = &qh->qhLink;
    qh->qhLink.next = &qh->qhLink;

    ehci->asyncQH = qh;

    // Periodic list queue setup
    qh           = EhciAllocQH(ehci);
    qh->qhlp     = PTR_TERMINATE;
    qh->ch       = 0;
    qh->caps     = 0;
    qh->curLink  = 0;
    qh->nextLink = PTR_TERMINATE;
    qh->altLink  = 0;
    qh->token    = 0;
    for (i=0; i<5; i++)
    {
        qh->buffer[i]    = 0;
        qh->extBuffer[i] = 0;
    }
    qh->transfer    = 0;
    qh->qhLink.prev = &qh->qhLink;
    qh->qhLink.next = &qh->qhLink;

    ehci->periodicQH = qh;
    for (i=0; i<1024; i++)
        ehci->frameList[i] = PTR_QH | (unsigned int)(void*)qh;
	
	// check the number of available companion controllers (n_cc)
	unsigned int n_cc = (ehci->capRegs->hcsparams) & HCSPARAMS_N_CC_MASK;
	printk("available companion controllers = %\n", n_cc);
	
    // Check extended capabilities
    unsigned int eecp = (ehci->capRegs->hccParams >> 8) & 0xFF;
    int msec = 1000;
    if (eecp >= 0x40)
    {
        // Disable BIOS legacy support
        legsup = Read((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp);
        if (legsup & USBLEGSUP_HC_BIOS)
        {
            printk("    In EHCI: disabling BIOS legacy support...");
            Write((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp, 0x01000000);
            while(msec > 0)
            {
                legsup = Read((PPCI)ehci->pci_pointer, ehci->bus, ehci->device, ehci->function, eecp);
                WaitMiliSecond(10);
                msec -= 10;
                if ((legsup & USBLEGSUP_HC_BIOS == 0))
                    break;
            }
            if (legsup & USBLEGSUP_HC_BIOS)
            {
                printk("disabling BIOS legacy support FAILED!\n");
                return;
            }
            else
                printk("disabling BIOS legacy support DONE!\n");
        }
    }

    // Disable interrupts
    ehci->opRegs->usbIntr          = 0;

    // Setup frame list
    ehci->opRegs->frameIndex       = 0;
    ehci->opRegs->periodicListBase = (unsigned int)(void*)ehci->frameList;
    ehci->opRegs->asyncListAddr    = (unsigned int)(void*)ehci->asyncQH;
    ehci->opRegs->ctrlDsSegment    = 0;

    // Clear status
    ehci->opRegs->usbSts = 0x3f;

    // Enable controller
    ehci->opRegs->usbCmd = (8 << CMD_ITC_SHIFT) | CMD_PSE | CMD_ASE | CMD_RS;
    while (ehci->opRegs->usbSts & STS_HCHALTED);

    // Configure all devices to be managed by the EHCI
    ehci->opRegs->configFlag = 1;
    WaitMiliSecond(5); 

    // Probe devices
    EhciProbe(ehci, &usb_device);
}

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
