#define QUEUE_SIZE 25
/* usable pins for hardware interrupt */
#define INT1 2
#define INT2 3

/* typedef for a void function pointer that represents a task */
typedef void (*TASK)(void);

/* structure of a queue */
typedef struct
{
  int size;
  int head;
  int tail;
  TASK* array;
} QUEUE;

// Queue global variable
QUEUE * queue = malloc(sizeof(QUEUE));

void setup() {
  // put your setup code here, to run once:
  QUEUE * queue = initQueue();
  Serial.begin(9600);

  // set up leds output
  pinMode(LED_BUILTIN, OUTPUT);
  // set up button for hardware interrupt (Inverted Logic since using the builtin pull up resistor)
  // 1 = not pressed, 0 = pressed
  pinMode(INT1, INPUT_PULLUP);
  pinMode(INT2, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(INT1), interrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(INT2), interrupt2, RISING);

  // Timer interrupt initialization (interrupt every 3000 ms)
  TCCR1A = 0;                           // Init Timer1A
  TCCR1B = 0;                           // Init Timer1B
  TCCR1B |= 0x5;                        // Prescaler = 1024
  TCNT1 = 18660;                        // Timer Preloading
  TIMSK1 |= 1;                          // enables timer overflow interrupt

}

void loop() {
  // do nothing while queue is empty
  while(isEmpty(queue))
  ;

  Serial.print("Current size = " );
  Serial.println(queue->size);
  TASK runTask = dequeue(queue);              // dequeue from head following FIFO structure
  Serial.print("Running Task , size = ");
  Serial.println(queue->size);
  runTask();                                  // execute function
}

void interrupt1(){
  enqueue(queue, &interrupt1Handler);
  Serial.println("button 1 interrupt added");
}

void interrupt1Handler(){
  Serial.println("Button 1 is pressed.");
}

void interrupt2(){
  enqueue(queue, &interrupt2Handler);
  Serial.println("button 2 interrupt added");
}

void interrupt2Handler(){
  Serial.println("Button 2 is pressed.");
}

/* ISR for timer1 overflow interrupt */
ISR(TIMER1_OVF_vect){
  TCNT1 = 18660; // Timer Preloading
  enqueue(queue, &functionTimerInterrupt);
  Serial.println("timer interrupt added");
}

// Toggles BUILTIN LED
void functionTimerInterrupt(){
  uint8_t oldSREG = SREG;                                         // save status register
  cli();                                                          // disable all interrupts
  PORTB ^= _BV(PORTB5);                                           // Toggle pin 5 port B (BUILTIN LED)
  SREG = oldSREG;                                                 // restore status register
}

/* Initialize queue structure */
QUEUE * initQueue(){
  queue->size = 0;
  queue->head = 0;
  queue->tail = 0;
  queue->array = (TASK *) malloc(sizeof(TASK) * QUEUE_SIZE);      // max queue size * sizeof(pointer) which is 1 byte
  return queue;
}

/* check if queue is full */
char isFull(QUEUE * queue){
  uint8_t oldSREG = SREG;                                         // save status register
  cli();                                                          // disable all interrupts
  char res = queue->size == QUEUE_SIZE;
  SREG = oldSREG;                                                 // write old status register to reenable interrupts
  return res;
}

/* check if queue is empty */
char isEmpty(QUEUE * queue){
  uint8_t oldSREG = SREG;                                         // save status register
  cli();                                                          // disable all interrupts
  char res = queue->size == 0;
  SREG = oldSREG;                                                 // write old status register to reenable interrupts
  return res;
}

/* add function pointer to a queue */
void enqueue(QUEUE * queue, TASK func){
  if(isFull(queue))
    return;

  uint8_t oldSREG = SREG;                                         // save status register
  cli();                                                          // disable all interrupts

  queue->tail = (queue->tail + 1) % QUEUE_SIZE;                // advance tail first if not empty (loops back to beginning with the help of modulo operator)
  queue->array[queue->tail] = func;                              // assign function pointer to queue
  queue->size = queue->size + 1;                                 // increase size
  
  SREG = oldSREG;                                                // write old status register to reenable interrupts
}

/* remove item from queue */
TASK dequeue(QUEUE * queue){
  if(isEmpty(queue)){
    Serial.println("Error");
    return NULL;
  }


  uint8_t oldSREG = SREG;                                         // save status register
  cli();                                                          // disable all interrupts
  
  TASK item = queue->array[queue->head];                          // retrieve the function
  queue->head = (queue->head + 1) % QUEUE_SIZE;                   // advance head
  queue->size = queue->size - 1;                                  // decrement size

  SREG = oldSREG;                                                 // write old status register to reenable interrupts
  return item;
}