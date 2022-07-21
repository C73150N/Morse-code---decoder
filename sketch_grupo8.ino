//grupo 8 - programacion para electronica 1 

// ( 512 + ruidodebajafrecuencias < Limite < 512 + señal )
int Limite = 700;
// palabra por minuto: velocidad
int ppm = 13;
// Tiempo estandar morse 
unsigned long tiempopunto = 1200 / ppm;   // tiempo en milisegundos
unsigned long tiemporaya = 3 * 1200 / ppm; // tiempo en milisegundos
unsigned long spaciopalabra = 7 * 1200 / ppm; //tiempo milisegundos
// pin analogico del microfono
const int pinanalogo = 0;       
unsigned long duracionconsonido = 0;    // tiempo de duracion del pulso
unsigned long duracionsinsonido = 0;   // tiempo de duracion del espacio
boolean espacioMorse = false;    // Prevenir multiples espacios recibidos Flag to prevent multiple received spaces
boolean ultimasenal = true;     // Señal de que el ultimo codigo recibido es punto o raya
//constantes arbol de decision morse
const int ultimoArbolMorse = 31;   // posicion ultima del arbol de decision morse.
int letrasMorsesalto = (ultimoArbolMorse+1)/2;
int letrasMorseNuevo = ultimoArbolMorse;
// Arbol de decisiones del codigo morse
char letrasMorse[] = "5H4S?V3I?F?U??2E?L?R???A?P?W?J1 6B?D?X?N?C?K?Y?T7Z?G?Q?M8??O9?0";
boolean  senalmorse = false;
unsigned long ultimaDemora = 0;  // ultimo tiempo que estuvo sin sonido


void setup()
{
  Serial.begin(9600);
  duracionconsonido = millis();
  duracionsinsonido = millis();
}

void loop()
{
    int audioMorse = analogRead(pinanalogo);
    unsigned long demora = millis();

    // Si hay señal analogica  mayor a el limite establecido...
    if (audioMorse > Limite)
    {
      // si es una señal nueva de codigo morse , resetea el tiempo a 0
      if (demora - ultimaDemora > tiempopunto/2)
      {
        duracionconsonido = demora;
        senalmorse = true; // hay actualmente una señal
      }
      ultimaDemora = demora;
    } else 
    {
      // si hay una pausa resetea el spacio de tiempo a 0 
      if (demora - ultimaDemora > tiempopunto/2 && senalmorse == true)
      {
        duracionsinsonido = ultimaDemora; // no muy lejos de la ultima señal recibida
        senalmorse = false;        // Sin señal
      }
    }

  // decoder de codigo morse con arbol de decision
  if (!senalmorse)
  {
    if (!ultimasenal)
    {
      if (letrasMorsesalto > 0)
      {
        // si la pausa es mayor que medio punto, recibe punto o raya considerando el mayor tiempo
        if (millis() - duracionsinsonido > tiempopunto/2)
        {
          // si la señal es mayor que 1/4 del itmpo del punto lo considera como pulso valido
          if (duracionsinsonido-duracionconsonido > tiempopunto/4)
          {
            // si la señal es menor que la mitad del tiempo de una raya lo toma como punto sino lo toma como raya
            if (duracionsinsonido-duracionconsonido < tiemporaya/2) letrasMorseNuevo -= letrasMorsesalto;
            else letrasMorseNuevo += letrasMorsesalto;
            letrasMorsesalto /= 2;
            ultimasenal = true;
          }
        }
      } else { // arroja error si existen una cola larga de pulsos
        Serial.println("<ERROR: señal no reconocida!>");
        ultimasenal = true;
        letrasMorsesalto = (ultimoArbolMorse+1)/2;
        letrasMorseNuevo = ultimoArbolMorse;
      }
    }
    // Escribe la letra si la pausa es mayor que 2/3 del tiempo (2 puntos) de la raya 
    if ((millis()-duracionsinsonido >= (tiempopunto*2)) && (letrasMorsesalto < 16))
    {
      char morseChar = letrasMorse[letrasMorseNuevo];
      Serial.print(morseChar);
      letrasMorsesalto = (ultimoArbolMorse+1)/2;
      letrasMorseNuevo = ultimoArbolMorse;
    }
    // Escribe un espacio si la pausa es mayor de 2/3 que la pausa de palabra
    if (millis()-duracionsinsonido > (spaciopalabra*2/3) && espacioMorse == false)
    {
      Serial.print(" ");
      espacioMorse = true ; 
    }
  } 
  else 
  {
    // si hay señal resetea variables booleanas
    ultimasenal = false;
    espacioMorse = false;
  }
}
