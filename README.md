# 🖥️ TecnoStore – Tienda de computadoras en C++ (POO)

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![raylib](https://img.shields.io/badge/raylib-GUI-orange)
![POO](https://img.shields.io/badge/paradigma-POO-success)
![Code::Blocks](https://img.shields.io/badge/IDE-Code%3A%3ABlocks-informational)

Aplicación de escritorio que simula una tienda de computadoras, desarrollada en **C++** con **Programación Orientada a Objetos** y una interfaz gráfica hecha con **raylib**.

> Proyecto del curso de Programación Orientada a Objetos – Escuela Profesional de Ingeniería de Sistemas, Universidad Nacional del Altiplano (UNA – Puno).

NOTA: El archivo principal se denomina main.cpp

---

## 📸 Capturas

(https://github.com/cristianchecalla-byte-unap/POO_TecnoStore/blob/main/Informe/Capturas%20e%20Imagenes/fig_menu_aplicacion.png)

## 🎬 Video de demostración

[[▶️ Ver el video](https://drive.google.com/file/d/168Y22QBmH3TcarbfVL0Tpzig_2Y6_P_i/view?usp=sharing)
]
---

## ✨ Funciones principales

- **Catálogo** de hardware, accesorios, periféricos, software, equipos completos y servicios, con búsqueda, filtros por categoría y marca, y ordenamiento por precio.
- **Modelos por producto** (por ejemplo, distintas GPU o capacidades de RAM) con su precio y un modelo recomendado.
- **Carrito** con control de stock y **promociones automáticas** (combo Mouse + Teclado y descuento por volumen).
- **Cupones de descuento** con compra mínima.
- **Pago en 4 pasos:** datos del cliente, comprobante y entrega, pago, y confirmación.
- **Medios de pago:** efectivo, tarjeta (+3 %), Yape y Plin.
- **Comprobantes:** boleta y factura (con RUC e IGV).
- **Envío:** recojo en tienda o delivery.
- **Pedidos** con comisión del vendedor.
- **Inventario:** agregar productos nuevos y reponer stock, guardado en un archivo.

## 🎟️ Cupones de prueba

| Código | Descuento | Condición |
|---|---|---|
| `TECNO10` | 10 % | Sin mínimo |
| `BIENVENIDO` | S/ 20 | Compra mínima de S/ 100 |
| `GAMER50` | S/ 50 | Compra mínima de S/ 1000 |
| `PRIMERA5` | 5 % | Sin mínimo |

---

## 🧱 Conceptos de POO aplicados

| Pilar | Dónde se ve |
|---|---|
| **Abstracción** | Clases abstractas `Producto`, `Persona`, `MedioPago`, `Comprobante`, `Envio`, `Promocion`, `Cupon`, `ElementoUI` |
| **Herencia** | `Producto → Componente → CPU, GPU, RAM…`; `Persona → Trabajador → Vendedor, Gerente…`; `MedioPago → Billetera → Yape, Plin` |
| **Polimorfismo** | `detalle()`, `precioFinal()`, `recargo()`, `validar()`, `comision()`, `emitir()` |
| **Encapsulamiento** | Atributos `protected`/`private` con *getters* (`stock()`, `precioBase()`…) |
| **Composición** | `Tienda` contiene catálogo, carrito, equipo, proveedores y cupones |

El código se divide en dos capas: **dominio** (reglas del negocio) e **interfaz** (pantallas), para que cada una pueda cambiar sin afectar a la otra.

---

## ⚙️ Requisitos

- Compilador con soporte de **C++17** (`g++` 9 o superior)
- Biblioteca **[raylib](https://www.raylib.com/)** instalada
- *(Windows, opcional)* **[Code::Blocks](https://www.codeblocks.org/)** como IDE, junto con **MSYS2** (ver la [guía de Code::Blocks](#-ejecutar-con-codeblocks-windows))

### Instalar raylib

**Linux (Ubuntu/Debian)**
```bash
sudo apt install libraylib-dev
```
Si tu distribución no la incluye, descárgala y compílala desde [github.com/raysan5/raylib](https://github.com/raysan5/raylib).

**Windows (MSYS2, terminal UCRT64)**
```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-raylib
pacman -S mingw-w64-ucrt-x86_64-make
```

**macOS**
```bash
brew install raylib
```

---

## 🚀 Compilar y ejecutar (terminal)

**Linux**
```bash
g++ -std=c++17 TiendaPOO_ordenado.cpp -o tienda -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./tienda
```

**Windows** (desde la terminal **MSYS2 UCRT64**)
```bash
g++ -std=c++17 TiendaPOO_ordenado.cpp -o tienda.exe -lraylib -lopengl32 -lgdi32 -lwinmm
./tienda.exe
```

> ⚠️ Crea una carpeta `img/` junto al ejecutable. Allí se guarda `inventario.dat`; sin ella, el stock no se conserva entre sesiones.

---

## 🧩 Ejecutar con Code::Blocks (Windows)

Si prefieres trabajar con un IDE, sigue estos pasos. **Todos son necesarios**, en especial los de MSYS2.

### 1. Instalar MSYS2
Descarga MSYS2 desde [msys2.org](https://www.msys2.org/) e instálalo en el directorio `C:\msys64` (ruta por defecto).

### 2. Instalar las dependencias
Abre **MSYS2 UCRT64** desde el menú Inicio y ejecuta:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-raylib
pacman -S mingw-w64-ucrt-x86_64-make
```

> Si la terminal se cierra tras `pacman -Syu`, ábrela de nuevo y continúa con los demás comandos.

### 3. Crear el proyecto
1. En Code::Blocks: **File → New → Project → Console application**.
2. Selecciona el lenguaje **C++**.
3. Elimina el `main.cpp` que se crea por defecto y agrega `TiendaPOO_ordenado.cpp` (clic derecho al proyecto → **Add files...**).

### 4. Configurar el compilador
Ve a **Settings → Compiler**:

1. En *Selected compiler*, deja el predeterminado: **GNU GCC Compiler**.
2. En **Compiler settings → Compiler Flags**, marca  
   `Have g++ follow the C++17 ISO C++ language standard [-std=c++17]`.
3. En la pestaña **Toolchain executables**, escribe en *Compiler's installation directory*:
   ```
   C:\msys64\ucrt64
   ```
4. Verifica los programas: `gcc.exe` (C compiler), `g++.exe` (C++ compiler) y `mingw32-make.exe` (make program). Presiona **OK**.

> ⚠️ Este paso **requiere** haber completado la instalación de MSYS2 (pasos 1 y 2).

### 5. Enlazar las librerías
1. Clic derecho al proyecto → **Build options...** (selecciona el nombre del proyecto, no *Debug* ni *Release*).
2. Entra a la pestaña **Linker settings**.
3. En **Link libraries**, presiona **Add** y agrega una por una (sin comillas, en este orden):
   ```
   raylib
   opengl32
   gdi32
   winmm
   ```
4. Presiona **OK**.

### 6. Compilar y ejecutar
Presiona **Build and Run** (`F9`) y listo. 🚀

### 💡 Solución de problemas

| Problema | Solución |
|---|---|
| Code::Blocks no encuentra raylib | En **Build options → Search directories** agrega `C:\msys64\ucrt64\include` (pestaña *Compiler*) y `C:\msys64\ucrt64\lib` (pestaña *Linker*). |
| Error *No Compiler* o no compila | Revisa que el paso 4 esté completo y que MSYS2 esté instalado en `C:\msys64`. |
| Tildes, ñ o símbolos se ven como `�` | **Settings → Editor → General settings → Encoding**: elige **UTF-8**. |
| El stock no se guarda entre sesiones | Crea la carpeta `img/` dentro de la carpeta del proyecto. |

---

## 🖼️ Imágenes opcionales

Si no hay imágenes, el programa dibuja ilustraciones propias.

- Fotos de productos: `img/productos/<codigo>.png` (por ejemplo `img/productos/CPU-01.png`)
- Logos de marcas: `img/marcas/<marca>.png` (por ejemplo `img/marcas/Intel.png`)

### Atajos

| Tecla | Acción |
|---|---|
| `F11` | Pantalla completa |
| `Esc` | Cierra ventanas o limpia la búsqueda |
| `Tab` | Cambia de casilla en el formulario de pago |
| `Enter` | Continúa al siguiente paso |

---

## 📁 Contenido del repositorio

```
.
├── TiendaPOO_ordenado.cpp      # Código fuente completo
├── Informe_TecnoStore.pdf      # Informe técnico (PDF)
├── Informe_TecnoStore.docx     # Informe técnico (Word)
├── Informe_TecnoStore.tex      # Informe técnico (LaTeX)
├── img/                        # Inventario, fotos y logos (opcional)
└── README.md
```

## 👥 Integrantes

| N.° | Apellidos y nombres | Código |
|---|---|---|
| 1 | Checalla Alata, Cristian Alex | 231503 |
| 2 | Balatro Balatrez | 123454 |
| 3 | Neko Baboso | 123429 |

**Docente:** Mr. Aceituno Deidad XD

---

## 📄 Licencia

Proyecto académico con fines educativos.
