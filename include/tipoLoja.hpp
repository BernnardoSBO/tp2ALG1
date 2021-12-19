#ifndef TIPO_LOJA
#define TIPO_LOJA

class tipoLoja {
  public:
  tipoLoja():x(0),y(0){}
  tipoLoja(int x, int y):x(x),y(y){}
  
  int getX();
  int getY();


  private:
  int x;
  int y;
};

#endif