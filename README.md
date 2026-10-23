# 🖥️ TecnoStore – Tienda de computadoras en C++ (POO)

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)
![raylib](https://img.shields.io/badge/raylib-GUI-orange)
![POO](https://img.shields.io/badge/paradigma-POO-success)

Aplicación de escritorio que simula una tienda de computadoras, desarrollada en **C++17** con **Programación Orientada a Objetos** y una interfaz gráfica hecha con **raylib**.

> Proyecto del curso de Programación Orientada a Objetos – Escuela Profesional de Ingeniería de Sistemas, Universidad Nacional del Altiplano (UNA – Puno).

---

## 📸 Capturas

<!-- Sube tus capturas a la carpeta docs/ y quita los comentarios -->
<!-- ![Inicio](docs/inicio.png) -->
<!-- ![Carrito](docs/carrito.png) -->
<!-- ![Pago](docs/pago.png) -->

## 🎬 Video de demostración

[▶️ Ver el video](PEGAR_AQUI_EL_ENLACE_DEL_VIDEO)

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

### Instalar raylib

**Linux (Ubuntu/Debian)**
```bash
sudo apt install libraylib-dev
```
Si tu distribución no la incluye, descárgala y compílala desde [github.com/raysan5/raylib](https://github.com/raysan5/raylib).

**Windows (MSYS2)**
```bash
pacman -S mingw-w64-x86_64-raylib
```

**macOS**
```bash
brew install raylib
```

## 🚀 Compilar y ejecutar

**Linux**
```bash
g++ -std=c++17 TiendaPOO_ordenado.cpp -o tienda -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./tienda
```

**Windows**
```bash
g++ -std=c++17 TiendaPOO_ordenado.cpp -o tienda.exe -lraylib -lopengl32 -lgdi32 -lwinmm
tienda.exe
```

> ⚠️ Crea una carpeta `img/` junto al ejecutable. Allí se guarda `inventario.dat`; sin ella, el stock no se conserva entre sesiones.

### Imágenes opcionales

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
| 1 | Checalla Alata, Cristian Alex| 231503 |
| 2 | Balatro Balatrez | 123454 |
| 3 | Neko Baboso | 123429 |

**Docente:** Mr. Aceituno Deidad XD

---

## 📄 Licencia

Proyecto académico con fines educativos.
