#include <16f877a.h>
#include <keyboard.h>

/************************************
* melhoria: return matrixKey[count][columns-1]; estudar como ajeita isso de forma
* mais simples. j� que o valor recebido � sempre pot�ncia de 2... ao inv�s do switch case.
*************************************/

//vari�veis auxiliares
uint8_t counter=0; 
//uint8_t button = 0xFF;//variavel preenchida com um valor padrao

uint8_t matrixKey[4][4] = { // posi��o das teclas no teclado matricial.
  {0x07, 0x08, 0x09, 0x0F},
  {0x04, 0x05, 0x06, 0x0E},
  {0x01, 0x02, 0x03, 0x0D},
  {0x0A, 0x00, 0x0B, 0x0C}
};

void key_init()
{
 // usar port B, de 0 a 3 como sa�das e de 4 a 7 como entradas.
 #use fixed_io (b_outputs = pin_b0,pin_b1,pin_b2,pin_b3)
}

// ler o teclado. A ideia � escrever numa linha, ler todas as colunas e
// devolver o valor referente a tecla lida.
EKey_status key_read(uint8_t &key_value)
{
   EKey_status button = NO_KEY_PRESSED;
  switch(counter)
  {
     case 0: output_b(0x01); // "ativa" a primeira linha
             button = decodKey(counter,key_value); // ler se alguma tecla foi pressionada
             counter = 1; // incrementa o contador...
     break;
     
     case 1: output_b(0x02); // "ativa" a segunda linha
             button = decodKey(counter,key_value);
             counter = 2;
     break;
     
     case 2: output_b(0x04); // "ativa" a terceira linha
             button = decodKey(counter,key_value);
             counter = 3;
     break;
     
     case 3: output_b(0x08);// "ativa" a quarta linha
             button = decodKey(counter,key_value);
             counter = 0;
     break;
  }
  return button;
}

EKey_status decodKey(uint8_t count, uint8_t &value)
{
   uint8_t columns = input_b();  // eh lido se alguma tecla fora pressionada
   columns = (columns & 0xF0) >> 4;  // mascara e deslocamento para descartar os bits menos significativos. pois correspondem ao pinos de saida
   if(columns != 0x00)  //se algo foi lido, haja vista que ler zero, � sinonimo de n�o terlido nada...
   {
      //return matrixKey[count][columns-1];       // procurar melhorar isso...
      switch(columns)
      {
         case 1: columns = 0; break;
         case 2: columns = 1; break;
         case 4: columns = 2; break;
         case 8: columns = 3; break;
      }
      value = matrixKey[count][columns];    // tecla que fora pressionada
      return ANY_KEY_WAS_PRESSED;
   }
   else
   {
      return NO_KEY_PRESSED; // n�o leu nada...
     // return 0;
   }
}

