//MELHORAR ESTE C�DIGO, DESEMPENHO EST� UMA MERDA.
#include <main.h>

#include<typedef.h>
#include<utils.h>
#include<Light.h>
#include<adc.h>

#define LIGHT_PIN PIN_B0

TLuminositySensor LumSensor = {0, LOW_LVL_LIGHT}; // struct with luminosity sensor parameters

TPresenceSensor PreSensor = {1, SENSOR_OFF}; // struct with presence sensor parameters

uint8_t time      = 0;
bool    IsTimerOn = FALSE;
   
#INT_TIMER1
void timerISR()
{
   time++;

      if (time >= 50)            // 20  = aprox 2s
      {
         IsTimerOn = FALSE;
         time = 0;
         light(LIGHT_PIN, LIGHT_OFF); // after x seconds spent, turn lights off
      }
      else
      {
        IsTimerOn = TRUE;
      }
}

void main()
{
   EMachineState   MachineState = INITIAL_STATE;  // Initial state of machine of states.
     
   setup_adc(ADC_CLOCK_DIV_32);   //configura conversor AD
   setup_adc_ports(AN0_AN1_AN3);   //escolhe pinos de entradas analogicas
   
   Setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);         //timer1 incrementado pelo clock interno e com prescaler 1:8
   
   
   while(TRUE)
   {
      set_adc_channel(LumSensor.channel);         //escolhe canal
      delay_ms(20);            //espera um tempo para a tens?o se estabilizar
      LumSensor.valor = luminositySensor(read_adc()); 
     
      set_adc_channel(PreSensor.ch);         //escolhe canal
      delay_ms(20);            //espera um tempo para a tens?o se estabilizar
      PreSensor.value = presenceSensor(read_adc());
      
      switch(MachineState) //Machine State of program
      {
         case INITIAL_STATE: 
            light(LIGHT_PIN, LIGHT_OFF); // Turn off lights
            
            if(LumSensor.valor == LOW_LVL_LIGHT)  
            {
               MachineState = LOW_LUMINOSITY_STATE;
            }
            else if(LumSensor.valor == HIGH_LVL_LIGHT)
            {
               MachineState = HIGH_LUMINOSITY_STATE;
            }
            break;
           
         case LOW_LUMINOSITY_STATE: //baixa luminosidade, pode ligar a luz ou n�o.
            if((PreSensor.value == SENSOR_ON) && (IsTimerOn == FALSE))
            {
               //dispararTimer Interrupt
               set_timer1(0);
               enable_interrupts(global);      //para colocar 2 interrup??es no mesmo operador elas t?m que estar no mesmo registrador de controle
               enable_interrupts(int_timer1);            //habilita a interrup??o de timer1

               light(LIGHT_PIN, LIGHT_ON);
               IsTimerOn = TRUE; 
            }
                   
            if(LumSensor.valor == HIGH_LVL_LIGHT)
            {
               MachineState = HIGH_LUMINOSITY_STATE;
            }          
            break;
            
         case HIGH_LUMINOSITY_STATE:
            light(LIGHT_PIN, LIGHT_OFF);
            //disable_interrupts(global);
            if(LumSensor.valor == LOW_LVL_LIGHT)
            {
               MachineState = LOW_LUMINOSITY_STATE;
            }          
            break;
      }
   }

}

