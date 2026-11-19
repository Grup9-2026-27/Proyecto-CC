INFORMACIÓN SOBRE LA ENTREGA:

El objetivo de este proyecto es desarrollar una simulación de un sistema de comunicación entre un satélite y una estación de tierra mediante el uso de programación en Python y Arduino. El sistema permite simular el envío de información sobre las condiciones de temperatura y humedad del satélite, así como la comunicación entre ambos dispositivos.

Para llevar a cabo el proyecto, se han desarrollado tres códigos diferentes: uno en Python y dos en Arduino. Cada uno de ellos desempeña una función específica dentro del sistema y, en conjunto, permiten establecer la comunicación entre el satélite, la estación terrestre y el ordenador.

El primer código de Arduino corresponde al Arduino Satélite, que actúa como transmisor. Su función principal es recopilar los datos de temperatura, humedad y distancia y enviarlos a través del canal de comunicación mySerial. De esta manera, el satélite puede transmitir la información a la estación terrestre.

El segundo código corresponde al Arduino Tierra, que funciona como receptor. Este dispositivo se comunica con el satélite, recibe los datos transmitidos mediante mySerial y permite que la información llegue al ordenador para su posterior procesamiento y visualización. También incluye el sistema de alarmas, que consisten en el encendido de un led si se detectan fallos en la comunicación entre Arduinos, en el sensor de temperatura y humedad o en el sensor ultrasónico (cada tipo de fallo tiene su propio led), además de la activación de una alarma sonora si el usuario no muestra señales de haber visto el aviso que aparece en la interfaz grafica.

Por último, el código desarrollado en Python establece la comunicación entre el ordenador y el Arduino Tierra. Además, se encarga de configurar y controlar una interfaz gráfica de usuario, diseñada para facilitar la interacción con el sistema. Mediante esta interfaz el usuario puede consultar y controlar la información recibida además de confirmar la visualización de las alarmas. Algunos de los controles que aparecen en esta interfaz son la posibilidad de iniciar, pausar y reanudar la entrada de datos del sensor de humedad y temperatura y decidir donde se realiza el cálculo de la media de temperaturas.

Además de los tres códigos principales, el proyecto incluye cuatro tests unitarios cuyo objetivo es comprobar que los diferentes componentes de los códigos de Arduino funcionan correctamente y que las conexiones necesarias están bien establecidas. Estos tests permiten verificar el funcionamiento de los elementos individuales del sistema y detectar posibles errores antes de realizar pruebas del conjunto completo.

Los cuatro tests unitarios desarrollados en la versión 1 del proyecto son los siguientes:

- Comunicación Placa-Portátil: comprueba que la comunicación entre la placa Arduino y el ordenador es estable y funciona correctamente.

- Comunicación entre Arduinos: verifica que la transmisión de datos entre el Arduino Satélite y el Arduino Tierra es estable y funciona correctamente.

- LEDs: comprueba que los LED funcionan correctamente y están bien conectados a las placas Arduino.

- Sensor de temperatura y humedad: verifica que el sensor funciona correctamente y está bien conectado, permitiendo obtener los datos ambientales necesarios para la simulación.

En la versión 2 se han desarrollado nuevos tests unitarios para comprobar el correcto funcionamiento de las elementos incorporados en esta etapa, estructurados en dos tests individuales y una carpeta específica dedicada al sistema de alarmas que agrupa tres tests distintos:

- Sensor ultrasónico: comprueba que el sensor de distancia funciona correctamente y está bien conectado, permitiendo realizar mediciones fiables.

- Integración del sensor ultrasónico con el servomotor: verifica que el barrido del servomotor y la toma de lecturas del sensor ultrasónico funcionan de manera conjunta y sincronizada, sin interferencias entre ambos componentes.

- Sistema de alarmas (carpeta con tres tests): comprueba el correcto funcionamiento de los avisos visuales y acústicos ante distintas fallos del sistema a través de tres pruebas independientes:
  - Alarma por fallo de comunicación: verifica la activación de la alerta cuando se interrumpe la transmisión de datos entre el satélite y la estación de tierra.
 
  - Alarma por fallo del sensor de temperatura y humedad: comprueba que el sistema detecta y notifica correctamente un error de lectura en el sensor.

  - Alarma por fallo del sensor ultrasónico: valida que el sistema dispara el aviso correspondiente ante fallos o lecturas fuera de rango en el sensor de distancia.

La integración de los tres códigos constituye la base del funcionamiento del proyecto. Para conseguirlo, es necesario que los dos programas de Arduino gestionen correctamente la transmisión y recepción de los datos, mientras que Python se encarga de interpretar la información recibida y presentarla de manera visual y accesible. Asimismo, los tests unitarios permiten comprobar por separado el funcionamiento de los componentes principales y facilitan la identificación de posibles fallos.

ACLARACIÓN DEL USO DE LA IA:

Durante el desarrollo de este proyecto, nuestro equipo ha usado la Inteligencia Artificial como apoyo de aprendizaje. La hemos usado principalmente en los pasos con un nivel más elevado, donde la comunicación entre placas y la integración del sistema requería soluciones específicas.
El uso de la IA se ha estructurado en los siguientes puntos clave:
- Comprensión e integración de funciones no mencionadas en la guía: La IA nos ha servido para implementar funciones esenciales que garantizaran la estabilidad de la comunicación y el procesamiento de datos en Arduino, las cuales teníamos que encontrar por nuestra cuenta. 
  En la primera versión destacan:
  
  - mensajePC.trim(): Utilizada para limpiar espacios en blanco y caracteres invisibles de salto de línea de los mensajes recibidos antes de procesarlos.

  - mySerial.readStringUntil('\n'): Para asegurar una lectura completa de los comandos procedentes del PC hasta encontrar el carácter de fin de línea.

  En la segunda versión:
  - comando.indexOf( , ): En nuestro caso usada para buscar la posición del carácter ':' que separa código y valor.

  - comando.substring( , ).toInt(): Para extraer y convertir a un número entero el código de la orden recibida.

  - enviarMensaje(" " + String() + " " + String() + " " + String()): Para enviar un string a partir de una función de una manera más eficiente.

- Resolución de bloqueos en algunos pasos: En las etapas más avanzadas, recurrimos a la IA cuando no sabíamos qué función o estructura utilizar. 

- Detección de errores: Utilizamos la IA como asistente de depuración cuando el código presentaba fallos. La IA nos ayudó a identificar posibles orígenes del problema y nos ofreció alternativas para solucionarlos.

En conclusión, la Inteligencia Artificial ha actuado como una herramienta de ayuda cuando encontrábamos errores de código y nos ha permitido entender en profundidad el funcionamiento de este.

ENLACE DE VÍDEO


