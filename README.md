INTRODUCCIÓN:

Somos el grupo 9, integrado por Pau Gabrysiak, Néstor Lombarte e Irati Vinyets. Esta entrega es la Versión 1 del proyecto realizado en clase.

<img width="303.2" height="202.4" alt="IMG_4285" src="https://github.com/user-attachments/assets/50ac5124-b5fe-4102-b2ae-ab16fe377565" />

INFORMACIÓN SOBRE LA ENTREGA:

El objetivo de este proyecto es desarrollar una simulación de un sistema de comunicación entre un satélite y una estación de tierra mediante el uso de programación en Python y Arduino. El sistema permite simular el envío de información sobre las condiciones de temperatura y humedad del satélite, así como la comunicación entre ambos dispositivos.

Para llevar a cabo el proyecto, se han desarrollado tres códigos diferentes: uno en Python y dos en Arduino. Cada uno de ellos desempeña una función específica dentro del sistema y, en conjunto, permiten establecer la comunicación entre el satélite, la estación terrestre y el ordenador.

El primer código de Arduino corresponde al Arduino Satélite, que actúa como transmisor. Su función principal es recopilar los datos de temperatura y humedad y enviarlos a través del canal de comunicación mySerial. De esta manera, el satélite puede transmitir la información a la estación terrestre.

El segundo código corresponde al Arduino Tierra, que funciona como receptor. Este dispositivo se comunica con el satélite, recibe los datos transmitidos mediante mySerial y permite que la información llegue al ordenador para su posterior procesamiento y visualización.

Por último, el código desarrollado en Python establece la comunicación entre el ordenador y el Arduino Tierra. Además, se encarga de configurar y controlar una interfaz gráfica de usuario, diseñada para facilitar la interacción con el sistema. Mediante esta interfaz el usuario puede consultar y controlar la información recibida.

Además de los tres códigos principales, el proyecto incluye cuatro tests unitarios cuyo objetivo es comprobar que los diferentes componentes de los códigos de Arduino funcionan correctamente y que las conexiones necesarias están bien establecidas. Estos tests permiten verificar el funcionamiento de los elementos individuales del sistema y detectar posibles errores antes de realizar pruebas del conjunto completo.

Los cuatro tests unitarios desarrollados son los siguientes:

- Comunicación Placa-Portátil: comprueba que la comunicación entre la placa Arduino y el ordenador es estable y funciona correctamente.

- Comunicación entre Arduinos: verifica que la transmisión de datos entre el Arduino Satélite y el Arduino Tierra es estable y funciona correctamente.

- LEDs: comprueba que los LED funcionan correctamente y están bien conectados a las placas Arduino.

- Sensor de temperatura y humedad: verifica que el sensor funciona correctamente y está bien conectado, permitiendo obtener los datos ambientales necesarios para la simulación.

La integración de los tres códigos constituye la base del funcionamiento del proyecto. Para conseguirlo, es necesario que los dos programas de Arduino gestionen correctamente la transmisión y recepción de los datos, mientras que Python se encarga de interpretar la información recibida y presentarla de manera visual y accesible. Asimismo, los tests unitarios permiten comprobar por separado el funcionamiento de los componentes principales y facilitan la identificación de posibles fallos.

ACLARACIÓN DEL USO DE LA IA:

Durante el desarrollo de esta versión del proyecto, nuestro equipo ha usado la Inteligencia Artificial como apoyo de aprendizaje. La hemos usado principalmente en los pasos más avanzados, donde la comunicación entre placas y la integración del sistema requería soluciones específicas.
El uso de la IA se ha estructurado en los siguientes puntos clave:
- Comprensión e integración de funciones no mencionadas en la guía: La IA nos ha servido para implementar funciones esenciales que garantizaran la estabilidad de la comunicación y el procesamiento de datos en Arduino, las cuales teníamos que encontrar por nuestra cuenta. Entre ellas destacan:
  - mensajePC.trim(): Utilizada para limpiar espacios en blanco y caracteres invisibles de salto de línea de los mensajes recibidos antes de procesarlos.
  - mySerial.readStringUntil('\n'): Para asegurar una lectura completa de los comandos procedentes del PC hasta encontrar el carácter de fin de línea.

- Resolución de bloqueos en pasos finales: En las etapas más avanzadas (como la implementación del control de flujo de inicio, pausa y reanudación), recurrimos a la IA cuando no sabíamos qué función o estructura utilizar. 

- Detección de errores: Utilizamos la IA como asistente de depuración cuando el código presentaba fallos. La IA nos ayudó a identificar posibles orígenes del problema y nos ofreció alternativas para solucionarlos.

- Comentarios: Hemos utilizado la Inteligencia Artificial para ayudarnos a redactar los comentarios del código de Python ya que, debido a su complejidad, consideramos que requería explicaciones detalladas. Para ello, hemos consultado el funcionamiento de algunas funciones y utilizado las explicaciones obtenidas para documentar el código de forma clara y extensa, facilitando su comprensión.

En conclusión, la Inteligencia Artificial ha actuado como una herramienta de ayuda cuando encontrábamos errores de código y nos ha permitido entender en profundidad el funcionamiento de este.

ENLACE DE VÍDEO

https://drive.google.com/file/d/1pXefoQ6epbVQrOdJW47xA1U6bL2vYG8u/view?usp=drive_link
