// =====================================================================================
// TIENDA DE COMPUTADORAS "TecnoStore" - C++17 (POO) + raylib      (versión legible)
// =====================================================================================
// Controles : F11 = pantalla completa · ESC = cierra ventanas / limpia la búsqueda
// Imágenes opcionales:  img/productos/<codigo>.png   y   img/marcas/<marca>.png
//
// Compilar:
//   Linux  : g++ -std=c++17 tienda_legible.cpp -o tienda -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
//   Windows: g++ -std=c++17 tienda_legible.cpp -o tienda.exe -lraylib -lopengl32 -lgdi32 -lwinmm
//
// Cupones: TECNO10 (10%) · BIENVENIDO (S/ 20, desde S/ 100) · GAMER50 (S/ 50, desde S/ 1000)
//          PRIMERA5 (5%) · ACEITUNO (90%)
//
// Conceptos de POO usados:
//   Abstracción     : Producto, Persona, MedioPago, Comprobante, Envio, Promocion, Cupon, ElementoUI
//   Encapsulamiento : atributos protected/private + getters
//   Herencia        : Producto > Componente > CPU...   |   Persona > Trabajador > Vendedor...
//   Polimorfismo    : precioFinal, detalle, recargo, validar, emitir, comision, calcular, dibujar
//   Composición     : Tienda tiene Catálogo, Carrito, Equipo, Cupones
//   SOLID           : un nuevo medio de pago o cupón = una nueva clase (OCP);
//                     el dominio (clases) está separado de la interfaz (SRP)
//
// ÍNDICE (busca estos títulos con Ctrl+F)
//   1. UTILIDADES BÁSICAS
//   2. DOMINIO: PERSONAS
//   3. DOMINIO: PRODUCTOS
//   4. DOMINIO: PAGO, COMPROBANTE, ENVÍO, PROMOCIONES, CUPONES, CARRITO
//   5. DOMINIO: TIENDA
//   6. INTERFAZ: ayudas de dibujo
//   7. INTERFAZ: ilustraciones de productos (arte)
//   8. INTERFAZ: elementos y pantallas (clase Interfaz)
//   9. MAIN
// =====================================================================================

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <bits/stdc++.h>
using namespace std;

// =====================================================================================
// 1. UTILIDADES BÁSICAS
// =====================================================================================

// Da formato de dinero: 12.5 -> "S/ 12.50"
static string dinero(double valor) {
    return TextFormat("S/ %.2f", valor);
}

static string minusc(string texto) {
    for (auto& letra : texto)
        letra = (char)tolower((unsigned char)letra);
    return texto;
}

static string mayusc(string texto) {
    for (auto& letra : texto)
        letra = (char)toupper((unsigned char)letra);
    return texto;
}

// Parte un texto por un separador: "a|b|c" -> {"a","b","c"}
static vector<string> dividir(const string& texto, char separador = '|') {
    vector<string> partes;
    stringstream flujo(texto);
    string parte;
    while (getline(flujo, parte, separador))
        partes.push_back(parte);
    return partes;
}

// =====================================================================================
// 2. DOMINIO: PERSONAS
//    Persona (abstracta) > Cliente / Proveedor / Trabajador > Cajero, Vendedor, Tecnico, Gerente
// =====================================================================================

struct DatosPersona {
    string nombres, apellidoPaterno, apellidoMaterno, telefono, correo, dni;
};

class Persona {
protected:
    DatosPersona datos_;

public:
    explicit Persona(DatosPersona datos) : datos_(move(datos)) {
    }

    virtual ~Persona() = default;

    string nombreCompleto() const {
        return datos_.nombres + " " + datos_.apellidoPaterno + " " + datos_.apellidoMaterno;
    }

    const DatosPersona& datos() const {
        return datos_;
    }

    virtual string rol() const = 0;  // cada tipo de persona describe su rol
};

class Cliente : public Persona {
    string tipo_;       // "persona" o "empresa"
    string direccion_;

public:
    Cliente(DatosPersona datos, string tipo, string direccion)
        : Persona(move(datos)), tipo_(move(tipo)), direccion_(move(direccion)) {
    }

    string rol() const override {
        return "Cliente " + tipo_;
    }

    const string& direccion() const {
        return direccion_;
    }
};

class Proveedor : public Persona {
    string razonSocial_, ruc_, alcance_, pais_;

public:
    Proveedor(DatosPersona datos, string razonSocial, string ruc, string alcance, string pais)
        : Persona(move(datos)), razonSocial_(move(razonSocial)), ruc_(move(ruc)), alcance_(move(alcance)),
          pais_(move(pais)) {
    }

    string rol() const override {
        return "Proveedor " + alcance_;
    }

    string razon() const {
        return razonSocial_;
    }

    string detalle() const {
        return "RUC " + ruc_ + " · " + pais_;
    }
};

class Trabajador : public Persona {
    string codigo_;
    double sueldo_;

public:
    Trabajador(DatosPersona datos, string codigo, double sueldo)
        : Persona(move(datos)), codigo_(move(codigo)), sueldo_(sueldo) {
    }

    const string& codigo() const {
        return codigo_;
    }

    double sueldo() const {
        return sueldo_;
    }

    // Por defecto un trabajador no gana comisión; Vendedor y Gerente lo sobrescriben.
    virtual double comision(double /*montoVenta*/) const {
        return 0;
    }
};

class Cajero : public Trabajador {
    string horaInicio_, horaFin_;

public:
    Cajero(DatosPersona datos, string codigo, double sueldo, string horaInicio, string horaFin)
        : Trabajador(move(datos), move(codigo), sueldo), horaInicio_(move(horaInicio)), horaFin_(move(horaFin)) {
    }

    string rol() const override {
        return "Cajero · " + horaInicio_ + " a " + horaFin_;
    }
};

class Vendedor : public Trabajador {
    double porcentajeComision_;

public:
    Vendedor(DatosPersona datos, string codigo, double sueldo, double porcentaje)
        : Trabajador(move(datos), move(codigo), sueldo), porcentajeComision_(porcentaje) {
    }

    string rol() const override {
        return TextFormat("Vendedor · comisión %.1f%%", porcentajeComision_);
    }

    double comision(double montoVenta) const override {
        return montoVenta * porcentajeComision_ / 100;
    }
};

class Tecnico : public Trabajador {
    string especialidad_;

public:
    Tecnico(DatosPersona datos, string codigo, double sueldo, string especialidad)
        : Trabajador(move(datos), move(codigo), sueldo), especialidad_(move(especialidad)) {
    }

    string rol() const override {
        return "Técnico · " + especialidad_;
    }
};

class Gerente : public Trabajador {
    double porcentajeComision_;

public:
    Gerente(DatosPersona datos, string codigo, double sueldo, double porcentaje)
        : Trabajador(move(datos), move(codigo), sueldo), porcentajeComision_(porcentaje) {
    }

    string rol() const override {
        return "Gerente";
    }

    double comision(double montoVenta) const override {
        return montoVenta * porcentajeComision_ / 100;
    }
};

// =====================================================================================
// 3. DOMINIO: PRODUCTOS
//    Producto (abstracto) > Componente / Accesorio / Periferico / Software / EquipoCompleto / Servicio
// =====================================================================================

// Un "modelo" es una variante del producto. El precio del modelo = precio base × factor.
struct Modelo {
    string nombre;
    double factor;
    bool recomendado;
};

// Lee modelos desde texto: "Nombre:factor|Nombre:factor*"   (el * marca el recomendado)
static vector<Modelo> parseModelos(const string& texto) {
    vector<Modelo> modelos;
    for (string item : dividir(texto)) {
        bool recomendado = !item.empty() && item.back() == '*';
        if (recomendado)
            item.pop_back();
        size_t posDosPuntos = item.rfind(':');
        modelos.push_back({item.substr(0, posDosPuntos), atof(item.substr(posDosPuntos + 1).c_str()), recomendado});
    }
    return modelos;
}

// Datos comunes que se necesitan para crear cualquier producto.
struct Ficha {
    string codigo, nombre, marca;
    double precio;
    int garantiaMeses, stock;
    string modelos = "Básico:0.8|Estándar:1*|Pro:1.6";
};

// Modelos sugeridos por marca (para productos NUEVOS). Una clave que empieza con "=" exige coincidencia exacta.
static vector<string> modelosDeMarca(const string& marca) {
    static const vector<pair<string, string>> tabla = {
        {"intel",
         "Core i3-12100|Core i5-12400F|Core i5-13400|Core i5-13600K|Core i7-13700HX|Core i7-13800HX|Core i7-14700K|Core i9-14900K"},
        {"amd",
         "Ryzen 5 5600|Ryzen 5 7600|Ryzen 7 5700X3D|Ryzen 7 7800X3D|Ryzen 7 9700X|Ryzen 9 7900X|Ryzen 9 9950X|Ryzen 9 7945HX"},
        {"nvidia", "RTX 3050|RTX 3060|RTX 4060|RTX 4060 Ti|RTX 4070 SUPER|RTX 4080 SUPER|RTX 5070|RTX 5080"},
        {"kingston",
         "FURY Beast DDR4 16 GB|FURY Beast DDR5 16 GB|FURY Renegade DDR5|NV2 1 TB|NV3 1 TB|KC3000 1 TB|A400 480 GB|DataTraveler 64 GB"},
        {"logitech", "G203 LIGHTSYNC|G305 LIGHTSPEED|G502 HERO|G502 X PLUS|PRO X SUPERLIGHT 2|MX Master 3S|G213 Prodigy|C920 HD"},
        {"asus",
         "Prime H610M|Prime B760M|TUF Gaming B650|TUF Gaming Z790|ROG Strix B760-F|ROG Strix Z790-E|ROG Strix X870-E|Vivobook 15"},
        {"corsair", "Vengeance DDR4 16 GB|Vengeance DDR5 32 GB|Dominator DDR5|CV550|RM750e|RM850x|K70 CORE|Katar Elite"},
        {"lenovo", "IdeaPad 1|IdeaPad 3|IdeaPad Slim 5|ThinkPad E14 Gen 6|ThinkPad T14|LOQ 15|Legion 5 Gen 9|Legion Pro 7i"},
        {"=hp", "250 G9|Pavilion 15|ProBook 440 G11|EliteBook 840|Victus 15|OMEN 16|Envy x360|LaserJet M111w"},
        {"epson",
         "EcoTank L3210|EcoTank L3250|EcoTank L4260|EcoTank L5590|EcoTank L6270|EcoTank L8050|WorkForce WF-2930|Expression XP-2200"},
        {"redragon", "K552 Kumara|K617 Fizz|K630 Dragonborn|M612 Predator|M711 Cobra|M913 Impact|H510 Zeus|S101"},
        {"seagate",
         "BarraCuda 1 TB|BarraCuda 2 TB|BarraCuda 4 TB|IronWolf 4 TB|IronWolf 8 TB|Expansion 2 TB|Expansion 5 TB|FireCuda 530 1 TB"},
        {"western digital",
         "Blue SN580|Green SN350|Black SN770|Black SN850X|Blue 1 TB HDD|Blue 2 TB HDD|Red Plus 4 TB|My Passport 2 TB"},
        {"=wd", "Blue SN580|Green SN350|Black SN770|Black SN850X|Blue 1 TB HDD|Blue 2 TB HDD|Red Plus 4 TB|My Passport 2 TB"},
        {"ugreen", "Revodok 5 en 1|Revodok 6 en 1|Hub USB 3.0|HDMI a VGA|USB-C 4K|Nexode 65 W|Nexode 100 W|Cable USB-C 1 m"},
        {"anker",
         "Nano 20 W|Nano 45 W|737 GaN 120 W|PowerCore 10K|PowerCore 20K|Soundcore 2|Soundcore Life Q30|Cable USB-C 1.8 m"},
        {"jbl", "Go 4|Clip 5|Flip 6|Charge 5|Xtreme 4|Boombox 3|PartyBox 110|Tune 520BT"},
        {"hyperx", "Cloud Stinger 2|Cloud III|Cloud Alpha|QuadCast S|QuadCast 2|SoloCast|Alloy Origins|Pulsefire Haste 2"}};

    string marcaMinuscula = minusc(marca);
    for (auto& [clave, modelos] : tabla) {
        bool exacta = clave[0] == '=';
        bool coincide = exacta ? marcaMinuscula == clave.substr(1) : marcaMinuscula.find(clave) != string::npos;
        if (coincide)
            return dividir(modelos);
    }
    // Marca desconocida: modelos genéricos
    return dividir(
        "Modelo estándar|Generación actual|Generación anterior|Versión Pro|Edición básica|Edición avanzada|Edición gamer|Edición profesional");
}

// Convierte los modelos elegidos en el formulario al formato de texto "Nombre:1*|Nombre:1".
static string textoModelosElegidos(const vector<string>& elegidos) {
    string resultado;
    for (size_t i = 0; i < elegidos.size(); i++)
        resultado += (resultado.empty() ? "" : "|") + elegidos[i] + ":1" + (i == 0 ? "*" : "");
    return resultado.empty() ? "Estándar:1*" : resultado;
}

// Listas amplias de modelos para los productos del catálogo (clave = código del producto).
static const unordered_map<string, string>& modelosAmpliados() {
    static const unordered_map<string, string> mapa = {
        {"CPU-01",
         "Core i3-12100:0.55|Core i5-12400F:0.7|Core i5-13400:1|Core i5-13600K:1.3|Core i7-12700K:1.35|Core i7-13700:1.45*|Core i7-13700K:1.65|Core i7-13700HX:1.5|Core i7-13800HX:1.6|Core i7-14700K:1.85|Core i9-13900:2|Core i9-13900HX:2.1|Core i9-14900K:2.4|Core Ultra 5 245K:1.35|Core Ultra 7 265K:1.9|Core Ultra 9 285K:2.7"},
        {"CPU-02",
         "Ryzen 5 5600:0.7|Ryzen 5 7600:1*|Ryzen 5 9600X:1.15|Ryzen 7 5700X3D:1.2|Ryzen 7 7700:1.25|Ryzen 7 9700X:1.6|Ryzen 7 7800X3D:1.9|Ryzen 9 7900X:2.1|Ryzen 9 7945HX:2.6|Ryzen 9 9950X:3.3"},
        {"GPU-02",
         "RX 6600:0.5|RX 7600:0.7|RX 7700 XT:1*|RX 7800 XT:1.3|RX 9070:1.5|RX 7900 GRE:1.6|RX 9070 XT:1.8|RX 7900 XTX:2.4"},
        {"GPU-01",
         "RTX 3050:0.5|RTX 3060:0.65|RTX 4060:0.8|RTX 4060 Ti:0.95|RTX 4070:1*|RTX 4070 SUPER:1.2|RTX 4070 Ti SUPER:1.55|RTX 4080 SUPER:1.8|RTX 4090:3|RTX 5060 Ti:1.05|RTX 5070:1.3|RTX 5070 Ti:1.7|RTX 5080:2.1|RTX 5090:3.6"},
        {"RAM-01", "8 GB DDR4:0.6|16 GB DDR4:1*|32 GB DDR4:1.9|8 GB DDR5:0.8|16 GB DDR5:1.3|32 GB DDR5:2.3|64 GB DDR5:4.2"},
        {"SSD-01", "120 GB:0.4|240 GB:0.7|480 GB:1*|1 TB:1.7|2 TB:3.2|4 TB:6"},
        {"SSD-02", "256 GB Gen3:0.7|512 GB Gen3:1*|1 TB Gen3:1.7|500 GB Gen4:1.3|1 TB Gen4:2.1|2 TB Gen4:3.8|4 TB Gen4:7"},
        {"HDD-01", "500 GB:0.7|1 TB:1|2 TB:1.5*|3 TB:2|4 TB:2.4|6 TB:3.4|8 TB:4.5"},
        {"MB-01", "H610:0.5|B660:0.7|B760:1*|B760 WiFi:1.2|Z690:1.4|Z790:1.8|Z790 WiFi:2.1|Z890:2.5"},
        {"PSU-01", "450 W:0.6|550 W:0.8|650 W:1*|750 W:1.25|850 W:1.6|1000 W:2.1|1200 W:2.8"},
        {"MOU-01", "Básico:0.8|Ergonómico:1*|Ligero:1.3|Gamer RGB:1.8|Pro inalámbrico:2.6"},
        {"MOU-02", "Básico:0.8|Estándar:1*|Gamer:1.7|Gamer RGB:2|Pro 16000 DPI:2.6"},
        {"TEC-01", "Membrana:0.8|Membrana RGB:1.1|Mecánico:1.4*|Mecánico RGB:2|Mecánico 75%:2.3|Inalámbrico:2.8"},
        {"PAR-01", "Mini:0.7|Estándar:1*|Potente:1.8|Barra de sonido:2.2"},
        {"CAM-01", "480p:0.4|720p:0.7|1080p:1*|1080p 60 fps:1.5|2K:1.8|4K:2"},
        {"PC-01", "Oficina:0.7|Hogar:1*|Estudio:1.2|Diseño:1.5|Gamer:1.9|Gamer Pro RTX:2.8"},
        {"LAP-01",
         "14 pulg Ryzen 5:0.7|15.6 pulg Core i5:0.95|15.6 pulg Ryzen 5:1*|15.6 pulg Core i7:1.3|16 pulg Ryzen 7:1.7|16 pulg Core i7 RTX 4060:2.3|16 pulg Core i9 RTX 4070:3.1"},
        {"PRN-01", "Inyección de tinta:0.7|Multifuncional:1*|Láser:1.6|Láser color:2.2"}};
    return mapa;
}

// Categorías de producto (el número se usa como índice en la interfaz)
enum Categoria { HARDWARE = 0, ACCESORIOS, PERIFERICOS, SOFTWARE, EQUIPOS, SERVICIOS };

// ---------------------------- clase base ----------------------------
class Producto {
protected:
    string codigo_, nombre_, marca_;
    double precioBase_;
    int categoria_, garantiaMeses_, stock_;
    vector<Modelo> modelos_;
    bool oferta_ = false;

public:
    Producto(const Ficha& ficha, int categoria)
        : codigo_(ficha.codigo), nombre_(ficha.nombre), marca_(ficha.marca), precioBase_(ficha.precio),
          categoria_(categoria), garantiaMeses_(ficha.garantiaMeses), stock_(ficha.stock),
          modelos_(parseModelos(ficha.modelos)) {
        if (ficha.precio < 0)
            throw invalid_argument("Precio invalido");
    }

    virtual ~Producto() = default;

    // ---- getters ----
    const string& codigo() const {
        return codigo_;
    }

    const string& nombre() const {
        return nombre_;
    }

    const string& marca() const {
        return marca_;
    }

    int categoria() const {
        return categoria_;
    }

    int garantia() const {
        return garantiaMeses_;
    }

    double precioBase() const {
        return precioBase_;
    }

    int stock() const {
        return stock_;
    }

    const vector<Modelo>& modelos() const {
        return modelos_;
    }

    // ---- modelos ----
    void fijarModelos(const string& texto) {
        auto nuevos = parseModelos(texto);
        if (!nuevos.empty())
            modelos_ = nuevos;
    }

    string modelosTexto() const {
        string resultado;
        for (const auto& m : modelos_)
            resultado += (resultado.empty() ? "" : "|") + m.nombre + ":" + TextFormat("%.4f", m.factor) +
                         (m.recomendado ? "*" : "");
        return resultado;
    }

    int recomendado() const {  // índice del modelo recomendado (0 si no hay)
        for (size_t i = 0; i < modelos_.size(); i++)
            if (modelos_[i].recomendado)
                return (int)i;
        return 0;
    }

    double precioModelo(int indice) const {
        size_t i = (size_t)Clamp((float)indice, 0, (float)modelos_.size() - 1);
        return precioBase_ * modelos_[i].factor;
    }

    // ---- stock ----
    bool disponible() const {
        return stock_ > 0;
    }

    string estado() const {
        return stock_ == 0 ? "Agotado" : stock_ <= 5 ? "Pocas unidades" : "Disponible";
    }

    void quitarStock(int cantidad) {
        stock_ = max(0, stock_ - cantidad);
    }

    void fijarStock(int cantidad) {
        stock_ = max(0, cantidad);
    }

    // ---- oferta y precio (polimórficos) ----
    void marcarOferta() {
        oferta_ = true;
    }

    virtual bool enOferta() const {
        return oferta_;
    }

    virtual double precioFinal(int modelo = 0) const {
        return precioModelo(modelo);
    }

    virtual string detalle() const = 0;  // cada producto describe sus especificaciones
};

// Producto creado por el usuario desde la pestaña Inventario.
class ProductoPersonalizado : public Producto {
    string detalle_;

public:
    ProductoPersonalizado(const Ficha& ficha, int categoria, string detalle)
        : Producto(ficha, categoria), detalle_(move(detalle)) {
    }

    string detalle() const override {
        return detalle_;
    }
};

// ---------------------------- Componentes (hardware) ----------------------------
class Componente : public Producto {
protected:
    int potenciaW_;

public:
    Componente(const Ficha& ficha, int potenciaW) : Producto(ficha, HARDWARE), potenciaW_(potenciaW) {
    }
};

class CPU : public Componente {
    int nucleos_, hilos_;
    double ghz_;

public:
    CPU(const Ficha& ficha, int potenciaW, int nucleos, int hilos, double ghz)
        : Componente(ficha, potenciaW), nucleos_(nucleos), hilos_(hilos), ghz_(ghz) {
    }

    string detalle() const override {
        return TextFormat("%d núcleos · %d hilos · %.1f GHz · %d W", nucleos_, hilos_, ghz_, potenciaW_);
    }
};

class GPU : public Componente {
    int vramGB_, nucleos_;
    double ghz_;

public:
    GPU(const Ficha& ficha, int potenciaW, int vramGB, double ghz, int nucleos)
        : Componente(ficha, potenciaW), vramGB_(vramGB), nucleos_(nucleos), ghz_(ghz) {
    }

    string detalle() const override {
        return TextFormat("%d GB VRAM · %.1f GHz · %d núcleos", vramGB_, ghz_, nucleos_);
    }
};

class RAM : public Componente {
    int capacidadGB_, mhz_;
    string tipoDDR_;

public:
    RAM(const Ficha& ficha, int potenciaW, int capacidadGB, int mhz, string tipoDDR)
        : Componente(ficha, potenciaW), capacidadGB_(capacidadGB), mhz_(mhz), tipoDDR_(move(tipoDDR)) {
    }

    string detalle() const override {
        return TextFormat("%d GB %s · %d MHz", capacidadGB_, tipoDDR_.c_str(), mhz_);
    }
};

class Almacenamiento : public Componente {
protected:
    int capacidadGB_, lecturaMBs_, escrituraMBs_;
    string formato_;

public:
    Almacenamiento(const Ficha& ficha, int potenciaW, int capacidadGB, int lectura, int escritura, string formato)
        : Componente(ficha, potenciaW), capacidadGB_(capacidadGB), lecturaMBs_(lectura), escrituraMBs_(escritura),
          formato_(move(formato)) {
    }

    string detalle() const override {
        return TextFormat("%s · %d GB · %d/%d MB/s", formato_.c_str(), capacidadGB_, lecturaMBs_, escrituraMBs_);
    }
};

class SSD : public Almacenamiento {
public:
    using Almacenamiento::Almacenamiento;

    string detalle() const override {
        return "SSD " + Almacenamiento::detalle();
    }
};

class HDD : public Almacenamiento {
public:
    using Almacenamiento::Almacenamiento;

    string detalle() const override {
        return "HDD " + Almacenamiento::detalle();
    }
};

class PlacaMadre : public Componente {
    string socket_, chipset_;
    int ranurasRAM_;

public:
    PlacaMadre(const Ficha& ficha, int potenciaW, string socket, string chipset, int ranurasRAM)
        : Componente(ficha, potenciaW), socket_(move(socket)), chipset_(move(chipset)), ranurasRAM_(ranurasRAM) {
    }

    string detalle() const override {
        return TextFormat("Socket %s · %s · %d ranuras RAM", socket_.c_str(), chipset_.c_str(), ranurasRAM_);
    }
};

class FuentePoder : public Componente {
    string certificacion_;

public:
    FuentePoder(const Ficha& ficha, int potenciaW, string certificacion)
        : Componente(ficha, potenciaW), certificacion_(move(certificacion)) {
    }

    string detalle() const override {
        return TextFormat("%d W · %s", potenciaW_, certificacion_.c_str());
    }
};

class Ventilador : public Componente {
    int rpm_, diametroMm_;

public:
    Ventilador(const Ficha& ficha, int potenciaW, int rpm, int diametroMm)
        : Componente(ficha, potenciaW), rpm_(rpm), diametroMm_(diametroMm) {
    }

    string detalle() const override {
        return TextFormat("%d RPM · %d mm", rpm_, diametroMm_);
    }
};

// ---------------------------- Accesorios ----------------------------
class Accesorio : public Producto {
protected:
    string compatibilidad_;

public:
    Accesorio(const Ficha& ficha, string compatibilidad) : Producto(ficha, ACCESORIOS), compatibilidad_(move(compatibilidad)) {
    }
};

class MemoriaFlash : public Accesorio {
    int capacidadGB_;
    string tipo_;

public:
    MemoriaFlash(const Ficha& ficha, string compatibilidad, int capacidadGB, string tipo)
        : Accesorio(ficha, move(compatibilidad)), capacidadGB_(capacidadGB), tipo_(move(tipo)) {
    }

    string detalle() const override {
        return TextFormat("%s %d GB · %s", tipo_.c_str(), capacidadGB_, compatibilidad_.c_str());
    }
};

class PastaTermica : public Accesorio {
    int gramos_;

public:
    PastaTermica(const Ficha& ficha, string compatibilidad, int gramos)
        : Accesorio(ficha, move(compatibilidad)), gramos_(gramos) {
    }

    string detalle() const override {
        return TextFormat("%d gramos · %s", gramos_, compatibilidad_.c_str());
    }
};

class Cable : public Accesorio {
    string conector_;
    double metros_;

public:
    Cable(const Ficha& ficha, string compatibilidad, string conector, double metros)
        : Accesorio(ficha, move(compatibilidad)), conector_(move(conector)), metros_(metros) {
    }

    string detalle() const override {
        return TextFormat("Conector %s · %.1f m", conector_.c_str(), metros_);
    }
};

class Adaptador : public Accesorio {
    string origen_, destino_;

public:
    Adaptador(const Ficha& ficha, string compatibilidad, string origen, string destino)
        : Accesorio(ficha, move(compatibilidad)), origen_(move(origen)), destino_(move(destino)) {
    }

    string detalle() const override {
        return origen_ + " a " + destino_ + " · " + compatibilidad_;
    }
};

class Cargador : public Accesorio {
    string conector_;
    int potenciaW_;

public:
    Cargador(const Ficha& ficha, string compatibilidad, string conector, int potenciaW)
        : Accesorio(ficha, move(compatibilidad)), conector_(move(conector)), potenciaW_(potenciaW) {
    }

    string detalle() const override {
        return TextFormat("%s · %d W · %s", conector_.c_str(), potenciaW_, compatibilidad_.c_str());
    }
};

// ---------------------------- Periféricos ----------------------------
class Periferico : public Producto {
    string conexion_;
    bool inalambrico_, bluetooth_;

protected:
    string conectividad() const {  // texto común: "USB · Inalámbrico · Bluetooth"
        return conexion_ + (inalambrico_ ? " · Inalámbrico" : "") + (bluetooth_ ? " · Bluetooth" : "");
    }

public:
    Periferico(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth)
        : Producto(ficha, PERIFERICOS), conexion_(move(conexion)), inalambrico_(inalambrico), bluetooth_(bluetooth) {
    }
};

class Mouse : public Periferico {
    int dpi_;
    bool rgb_;

public:
    Mouse(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, int dpi, bool rgb)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), dpi_(dpi), rgb_(rgb) {
    }

    string detalle() const override {
        return conectividad() + TextFormat(" · %d DPI%s", dpi_, rgb_ ? " · RGB" : "");
    }
};

class Teclado : public Periferico {
    string tipo_;
    bool rgb_;

public:
    Teclado(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, string tipo, bool rgb)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), tipo_(move(tipo)), rgb_(rgb) {
    }

    string detalle() const override {
        return tipo_ + " · " + conectividad() + (rgb_ ? " · RGB" : "");
    }
};

class Parlante : public Periferico {
    int potenciaW_;

public:
    Parlante(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, int potenciaW)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), potenciaW_(potenciaW) {
    }

    string detalle() const override {
        return conectividad() + TextFormat(" · %d W", potenciaW_);
    }
};

class CamaraWeb : public Periferico {
    int fps_;
    string resolucion_;

public:
    CamaraWeb(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, int fps, string resolucion)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), fps_(fps), resolucion_(move(resolucion)) {
    }

    string detalle() const override {
        return TextFormat("%s · %d fps · ", resolucion_.c_str(), fps_) + conectividad();
    }
};

class Microfono : public Periferico {
    string tipo_;

public:
    Microfono(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, string tipo)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), tipo_(move(tipo)) {
    }

    string detalle() const override {
        return tipo_ + " · " + conectividad();
    }
};

class AdaptadorInalambrico : public Periferico {
    string tecnologia_;

public:
    AdaptadorInalambrico(const Ficha& ficha, string conexion, bool inalambrico, bool bluetooth, string tecnologia)
        : Periferico(ficha, move(conexion), inalambrico, bluetooth), tecnologia_(move(tecnologia)) {
    }

    string detalle() const override {
        return tecnologia_ + " · " + conectividad();
    }
};

// ---------------------------- Software ----------------------------
class Software : public Producto {
    bool permanente_;
    int meses_;

protected:
    string licencia() const {
        return permanente_ ? string("Licencia permanente") : string(TextFormat("Licencia de %d meses", meses_));
    }

public:
    Software(const Ficha& ficha, bool permanente, int meses) : Producto(ficha, SOFTWARE), permanente_(permanente), meses_(meses) {
    }

    // Descuento del 5% por ser licencia digital
    double precioFinal(int modelo = 0) const override {
        return precioModelo(modelo) * 0.95;
    }

    bool enOferta() const override {
        return true;
    }
};

class SistemaOperativo : public Software {
    string edicion_;

public:
    SistemaOperativo(const Ficha& ficha, bool permanente, int meses, string edicion)
        : Software(ficha, permanente, meses), edicion_(move(edicion)) {
    }

    string detalle() const override {
        return "Sistema operativo " + edicion_ + " · " + licencia();
    }
};

class Aplicacion : public Software {
    string tipo_;

public:
    Aplicacion(const Ficha& ficha, bool permanente, int meses, string tipo)
        : Software(ficha, permanente, meses), tipo_(move(tipo)) {
    }

    string detalle() const override {
        return tipo_ + " · " + licencia();
    }
};

// ---------------------------- Equipos completos ----------------------------
class EquipoCompleto : public Producto {
    string procesador_, ram_, almacenamiento_;

protected:
    string especificaciones() const {
        return procesador_ + " · " + ram_ + " · " + almacenamiento_;
    }

public:
    EquipoCompleto(const Ficha& ficha, string procesador, string ram, string almacenamiento)
        : Producto(ficha, EQUIPOS), procesador_(move(procesador)), ram_(move(ram)), almacenamiento_(move(almacenamiento)) {
    }

    // El precio incluye la garantía: S/ 10 por cada mes
    double precioFinal(int modelo = 0) const override {
        return precioModelo(modelo) + garantiaMeses_ * 10.0;
    }
};

class Computadora : public EquipoCompleto {
    string gabinete_;

public:
    Computadora(const Ficha& ficha, string procesador, string ram, string almacenamiento, string gabinete)
        : EquipoCompleto(ficha, move(procesador), move(ram), move(almacenamiento)), gabinete_(move(gabinete)) {
    }

    string detalle() const override {
        return "Gabinete " + gabinete_ + " · " + especificaciones();
    }
};

class Laptop : public EquipoCompleto {
    double pulgadas_;
    int bateriaWh_;
    bool oled_;

public:
    Laptop(const Ficha& ficha, string procesador, string ram, string almacenamiento, double pulgadas, int bateriaWh, bool oled)
        : EquipoCompleto(ficha, move(procesador), move(ram), move(almacenamiento)), pulgadas_(pulgadas),
          bateriaWh_(bateriaWh), oled_(oled) {
    }

    string detalle() const override {
        return string(TextFormat("%.1f\" %s · %d Wh · ", pulgadas_, oled_ ? "OLED" : "LED", bateriaWh_)) + especificaciones();
    }
};

class Impresora : public EquipoCompleto {
    string tecnologia_;
    int paginasPorMinuto_;
    bool tieneEscaner_;

public:
    Impresora(const Ficha& ficha, string tecnologia, int paginasPorMinuto, bool tieneEscaner)
        : EquipoCompleto(ficha, "-", "-", "-"), tecnologia_(move(tecnologia)), paginasPorMinuto_(paginasPorMinuto),
          tieneEscaner_(tieneEscaner) {
    }

    string detalle() const override {
        return TextFormat("%s · %d ppm%s", tecnologia_.c_str(), paginasPorMinuto_, tieneEscaner_ ? " · Escáner" : "");
    }
};

// ---------------------------- Servicios ----------------------------
class Servicio : public Producto {
protected:
    string descripcion_;

public:
    Servicio(const Ficha& ficha, string descripcion) : Producto(ficha, SERVICIOS), descripcion_(move(descripcion)) {
    }

    string detalle() const override {
        return descripcion_;
    }
};

class Garantia : public Servicio {
    string diagnostico_;

public:
    Garantia(const Ficha& ficha, string descripcion, string diagnostico)
        : Servicio(ficha, move(descripcion)), diagnostico_(move(diagnostico)) {
    }

    string detalle() const override {
        return descripcion_ + " · Diagnóstico: " + diagnostico_;
    }
};

class Mantenimiento : public Servicio {
    int vigenciaMeses_;

public:
    Mantenimiento(const Ficha& ficha, string descripcion, int vigenciaMeses)
        : Servicio(ficha, move(descripcion)), vigenciaMeses_(vigenciaMeses) {
    }

    string detalle() const override {
        return descripcion_ + TextFormat(" · vigencia %d meses", vigenciaMeses_);
    }
};

// Precio más bajo entre todos los modelos ("Desde S/ ...")
static double desdePrecio(const Producto* producto) {
    double menor = producto->precioFinal(0);
    for (size_t i = 1; i < producto->modelos().size(); i++)
        menor = min(menor, producto->precioFinal((int)i));
    return menor;
}

// =====================================================================================
// 4. DOMINIO: PAGO, COMPROBANTE, ENVÍO, PROMOCIONES, CUPONES, CARRITO
// =====================================================================================

// ---------------------------- Medios de pago ----------------------------
class MedioPago {
public:
    virtual ~MedioPago() = default;
    virtual string nombre() const = 0;
    virtual string campo() const = 0;                                     // dato que se pide al pagar
    virtual string validar(const string& dato, double total) const = 0;   // "" = válido, si no: mensaje de error
    virtual string resumen(const string& dato, double total) const = 0;

    virtual double recargo(double /*monto*/) const {
        return 0;
    }
};

class Efectivo : public MedioPago {
public:
    string nombre() const override {
        return "Efectivo";
    }

    string campo() const override {
        return "Monto con el que pagas (S/)";
    }

    string validar(const string& dato, double total) const override {
        bool insuficiente = dato.empty() || atof(dato.c_str()) + 0.005 < total;
        return insuficiente ? "Debe cubrir al menos " + dinero(total) : "";
    }

    string resumen(const string& dato, double total) const override {
        double recibido = atof(dato.c_str());
        return "Efectivo · recibido " + dinero(recibido) + " · vuelto " + dinero(recibido - total);
    }
};

class Tarjeta : public MedioPago {
public:
    string nombre() const override {
        return "Tarjeta de crédito (+3%)";
    }

    string campo() const override {
        return "Últimos 4 dígitos de la tarjeta";
    }

    string validar(const string& dato, double) const override {
        return dato.size() == 4 ? "" : "Escribe los 4 últimos dígitos";
    }

    string resumen(const string& dato, double) const override {
        return "Tarjeta terminada en " + dato;
    }

    double recargo(double monto) const override {
        return monto * 0.03;
    }
};

// Base común de Yape y Plin: piden un celular de 9 dígitos que empiece con 9
class Billetera : public MedioPago {
    string nombre_;

public:
    explicit Billetera(string nombre) : nombre_(move(nombre)) {
    }

    string nombre() const override {
        return nombre_;
    }

    string campo() const override {
        return "Número de celular de " + nombre_;
    }

    string validar(const string& dato, double) const override {
        bool valido = dato.size() == 9 && dato[0] == '9';
        return valido ? "" : "Celular de 9 dígitos que empiece con 9";
    }

    string resumen(const string& dato, double) const override {
        return nombre_ + " · celular " + dato;
    }
};

class Yape : public Billetera {
public:
    Yape() : Billetera("Yape") {
    }
};

class Plin : public Billetera {
public:
    Plin() : Billetera("Plin") {
    }
};

// ---------------------------- Comprobantes ----------------------------
class Comprobante {
public:
    virtual ~Comprobante() = default;
    virtual string nombre() const = 0;
    virtual string serie() const = 0;

    virtual bool pideRUC() const {
        return false;
    }

    virtual string emitir(double total) const = 0;
};

class Boleta : public Comprobante {
public:
    string nombre() const override {
        return "Boleta";
    }

    string serie() const override {
        return "B001";
    }

    string emitir(double total) const override {
        return "Boleta · total " + dinero(total);
    }
};

class Factura : public Comprobante {
public:
    string nombre() const override {
        return "Factura";
    }

    string serie() const override {
        return "F001";
    }

    bool pideRUC() const override {
        return true;
    }

    string emitir(double total) const override {
        double igv = total - total / 1.18;
        return "Factura · total " + dinero(total) + " (IGV " + dinero(igv) + ")";
    }
};

// ---------------------------- Envíos ----------------------------
class Envio {
public:
    virtual ~Envio() = default;
    virtual string nombre() const = 0;
    virtual double costo() const = 0;

    virtual bool pideDireccion() const {
        return false;
    }
};

class RecojoEnTienda : public Envio {
public:
    string nombre() const override {
        return "Recojo en tienda";
    }

    double costo() const override {
        return 0;
    }
};

class Delivery : public Envio {
public:
    string nombre() const override {
        return "Delivery (+S/ 10)";
    }

    double costo() const override {
        return 10;
    }

    bool pideDireccion() const override {
        return true;
    }
};

// ---------------------------- Líneas del carrito y promociones ----------------------------
struct Linea {
    const Producto* producto;
    int modelo;
    int cantidad;
};

// Cuántas unidades de un producto (por nombre) hay en las líneas
static int cantidadPorNombre(const vector<Linea>& lineas, const string& nombre) {
    int total = 0;
    for (auto& l : lineas)
        if (l.producto->nombre() == nombre)
            total += l.cantidad;
    return total;
}

static double subtotalDe(const vector<Linea>& lineas) {
    double suma = 0;
    for (auto& l : lineas)
        suma += l.producto->precioFinal(l.modelo) * l.cantidad;
    return suma;
}

class Promocion {
public:
    virtual ~Promocion() = default;
    virtual string nombre() const = 0;
    virtual double descuento(const vector<Linea>& lineas) const = 0;
};

// Combo: por cada par (A + B) se descuenta un monto fijo
class PromoCombo : public Promocion {
    string productoA_, productoB_;
    double descuentoPorPar_;

public:
    PromoCombo(string productoA, string productoB, double descuentoPorPar)
        : productoA_(move(productoA)), productoB_(move(productoB)), descuentoPorPar_(descuentoPorPar) {
    }

    string nombre() const override {
        return "Combo " + productoA_ + " + " + productoB_;
    }

    double descuento(const vector<Linea>& lineas) const override {
        int pares = min(cantidadPorNombre(lineas, productoA_), cantidadPorNombre(lineas, productoB_));
        return pares * descuentoPorPar_;
    }
};

// Volumen: si llevas N o más productos, descuento porcentual sobre el subtotal
class PromoVolumen : public Promocion {
    int minimoProductos_;
    double porcentaje_;  // 0.05 = 5%

public:
    PromoVolumen(int minimoProductos, double porcentaje) : minimoProductos_(minimoProductos), porcentaje_(porcentaje) {
    }

    string nombre() const override {
        return TextFormat("Volumen: %d+ productos", minimoProductos_);
    }

    double descuento(const vector<Linea>& lineas) const override {
        int unidades = 0;
        for (auto& l : lineas)
            unidades += l.cantidad;
        return unidades >= minimoProductos_ ? subtotalDe(lineas) * porcentaje_ : 0;
    }
};

// ---------------------------- Cupones ----------------------------
// El cliente escribe el código al pagar (no distingue mayúsculas). Cada tipo calcula su descuento.
class Cupon {
protected:
    string codigo_, descripcion_;
    double minimoCompra_;

public:
    Cupon(string codigo, string descripcion, double minimoCompra)
        : codigo_(move(codigo)), descripcion_(move(descripcion)), minimoCompra_(minimoCompra) {
    }

    virtual ~Cupon() = default;

    const string& codigo() const {
        return codigo_;
    }

    const string& descripcion() const {
        return descripcion_;
    }

    double minimo() const {
        return minimoCompra_;
    }

    virtual double calcular(double neto) const = 0;

    // Descuento real: 0 si no llega al mínimo, y nunca más que el neto
    double descuento(double neto) const {
        if (neto + 1e-9 < minimoCompra_)
            return 0;
        return min(neto, calcular(neto));
    }
};

class CuponPorcentaje : public Cupon {
    double porcentaje_;

public:
    CuponPorcentaje(string codigo, string descripcion, double minimoCompra, double porcentaje)
        : Cupon(move(codigo), move(descripcion), minimoCompra), porcentaje_(porcentaje) {
    }

    double calcular(double neto) const override {
        return neto * porcentaje_ / 100;
    }
};

class CuponMonto : public Cupon {
    double monto_;

public:
    CuponMonto(string codigo, string descripcion, double minimoCompra, double monto)
        : Cupon(move(codigo), move(descripcion), minimoCompra), monto_(monto) {
    }

    double calcular(double) const override {
        return monto_;
    }
};

// ---------------------------- Carrito ----------------------------
class Carrito {
    vector<Linea> lineas_;
    vector<unique_ptr<Promocion>> promociones_;

public:
    void agregarPromo(unique_ptr<Promocion> promo) {
        promociones_.push_back(move(promo));
    }

    // Devuelve cuántas unidades se agregaron realmente (respeta el stock disponible)
    int agregar(const Producto* producto, int modelo, int cantidad = 1) {
        cantidad = min(cantidad, producto->stock() - cantidadDe(producto));
        if (cantidad <= 0)
            return 0;
        for (auto& l : lineas_)
            if (l.producto == producto && l.modelo == modelo) {
                l.cantidad += cantidad;
                return cantidad;
            }
        lineas_.push_back({producto, modelo, cantidad});
        return cantidad;
    }

    // Quita UNA unidad de la línea; si llega a 0 se elimina la línea
    void quitar(size_t indice) {
        if (indice < lineas_.size() && --lineas_[indice].cantidad <= 0)
            lineas_.erase(lineas_.begin() + indice);
    }

    void vaciar() {
        lineas_.clear();
    }

    const vector<Linea>& lineas() const {
        return lineas_;
    }

    int cantidad() const {  // total de unidades
        int total = 0;
        for (auto& l : lineas_)
            total += l.cantidad;
        return total;
    }

    int cantidadDe(const Producto* producto) const {  // unidades de un producto (todos sus modelos)
        int total = 0;
        for (auto& l : lineas_)
            if (l.producto == producto)
                total += l.cantidad;
        return total;
    }

    double subtotal() const {
        return subtotalDe(lineas_);
    }

    // Lista de (nombre de promoción, descuento) de las promociones que sí aplican
    vector<pair<string, double>> detalleDescuentos() const {
        vector<pair<string, double>> resultado;
        for (auto& promo : promociones_) {
            double d = promo->descuento(lineas_);
            if (d > 0)
                resultado.push_back({promo->nombre(), d});
        }
        return resultado;
    }

    double descuentos() const {
        double suma = 0;
        for (auto& d : detalleDescuentos())
            suma += d.second;
        return suma;
    }
};

// Registro de una compra ya pagada
struct Pedido {
    string numero, fecha, cliente, documento, entrega, vendedor, pago, comprobante, cupon;
    double total = 0, comision = 0;
    vector<string> items;
};

// =====================================================================================
// 5. DOMINIO: TIENDA  (une todo: catálogo, equipo, carrito, pagos e inventario)
// =====================================================================================
class Tienda {
    vector<unique_ptr<Producto>> catalogo_;
    vector<unique_ptr<MedioPago>> mediosPago_;
    vector<unique_ptr<Comprobante>> comprobantes_;
    vector<unique_ptr<Envio>> envios_;
    vector<unique_ptr<Trabajador>> equipo_;
    vector<unique_ptr<Proveedor>> proveedores_;
    vector<unique_ptr<Cupon>> cupones_;
    vector<Pedido> pedidos_;
    Carrito carrito_;
    const Cupon* cuponAplicado_ = nullptr;
    int medioElegido_ = 0, comprobanteElegido_ = 0, envioElegido_ = 0, contadorPedidos_ = 0;

    // Crea un producto de tipo T y lo agrega al catálogo
    template <class T, class... Args> void agregarAlCatalogo(Args&&... args) {
        catalogo_.push_back(make_unique<T>(forward<Args>(args)...));
    }

    template <class T> static vector<string> nombresDe(const vector<unique_ptr<T>>& lista) {
        vector<string> resultado;
        for (auto& elemento : lista)
            resultado.push_back(elemento->nombre());
        return resultado;
    }

    // Quita los caracteres que romperían el archivo de inventario
    static string limpio(string texto) {
        for (char& c : texto)
            if (c == '|' || c == '\t' || c == '\n' || c == '\r')
                c = ' ';
        return texto;
    }

    // ---- Persistencia: img/inventario.dat ----
    // Formato:  S|codigo|stock                       (stock de un producto del catálogo base)
    //           P|codigo|nombre|marca|cat|precio|garantia|stock|modelos|detalle   (producto creado por el usuario)
    void guardarInventario() const {
        ofstream archivo("img/inventario.dat", ios::trunc);
        if (!archivo)
            return;
        for (const auto& p : catalogo_) {
            if (dynamic_cast<const ProductoPersonalizado*>(p.get()))
                archivo << "P|" << limpio(p->codigo()) << '|' << limpio(p->nombre()) << '|' << limpio(p->marca()) << '|'
                        << p->categoria() << '|' << p->precioBase() << '|' << p->garantia() << '|' << p->stock() << '|'
                        << limpio(p->modelosTexto()) << '|' << limpio(p->detalle()) << '\n';
            else
                archivo << "S|" << limpio(p->codigo()) << '|' << p->stock() << '\n';
        }
    }

    void cargarInventario() {
        ifstream archivo("img/inventario.dat");
        string linea;
        while (getline(archivo, linea)) {
            stringstream flujo(linea);
            string tipo, codigo;
            getline(flujo, tipo, '|');
            getline(flujo, codigo, '|');
            try {
                if (tipo == "S") {
                    int stock = 0;
                    flujo >> stock;
                    for (auto& p : catalogo_)
                        if (p->codigo() == codigo)
                            p->fijarStock(stock);
                } else if (tipo == "P") {
                    string nombre, marca, categoria, precio, garantia, stock, modelos, detalle;
                    for (string* campo : {&nombre, &marca, &categoria, &precio, &garantia, &stock, &modelos})
                        getline(flujo, *campo, '|');
                    getline(flujo, detalle);

                    bool yaExiste = false;
                    for (const auto& p : catalogo_)
                        if (p->codigo() == codigo)
                            yaExiste = true;

                    if (!yaExiste && !nombre.empty() && !modelos.empty()) {
                        Ficha ficha{codigo, nombre, marca, atof(precio.c_str()), atoi(garantia.c_str()), atoi(stock.c_str()), modelos};
                        catalogo_.push_back(make_unique<ProductoPersonalizado>(ficha, Clamp(atoi(categoria.c_str()), 0, 5), detalle));
                    }
                }
            } catch (...) {
                // ignora solo la fila dañada
            }
        }
    }

    // ---- Carga inicial de datos de ejemplo ----
    void crearCatalogoBase() {
        // Componentes
        agregarAlCatalogo<CPU>(Ficha{"CPU-01", "CPU", "Intel", 650, 24, 12, "Core i5-13400:1|Core i7-13700:1.45*|Core i9-13900:2"}, 65, 16, 24, 3.4);
        agregarAlCatalogo<CPU>(Ficha{"CPU-02", "CPU", "AMD", 620, 24, 10, "Ryzen 5 7600:1*|Ryzen 7 7800X3D:1.9"}, 65, 6, 12, 4.7);
        agregarAlCatalogo<GPU>(Ficha{"GPU-01", "GPU", "Nvidia", 1800, 24, 6, "RTX 4060:0.8|RTX 4070:1*|RTX 4080:1.7"}, 200, 12, 2.5, 5888);
        agregarAlCatalogo<GPU>(Ficha{"GPU-02", "GPU", "AMD", 1500, 24, 5, "RX 7700 XT:1*|RX 7800 XT:1.3"}, 263, 16, 2.4, 3840);
        agregarAlCatalogo<RAM>(Ficha{"RAM-02", "Memoria RAM", "Corsair", 240, 12, 16, "8 GB:0.6|16 GB:1*|32 GB:1.9"}, 5, 16, 3600, "DDR4");
        agregarAlCatalogo<RAM>(Ficha{"RAM-01", "Memoria RAM", "Kingston", 220, 12, 20, "8 GB:0.6|16 GB:1*|32 GB:1.9"}, 5, 16, 3200, "DDR4");
        agregarAlCatalogo<SSD>(Ficha{"SSD-01", "SSD", "Kingston", 260, 36, 15, "240 GB:0.7|480 GB:1*|1 TB:1.6"}, 3, 480, 560, 530, "SATA 2.5\"");
        agregarAlCatalogo<SSD>(Ficha{"SSD-02", "M.2", "WD", 240, 36, 10, "256 GB:0.8|512 GB:1*|1 TB:1.7"}, 4, 512, 3500, 3000, "M.2 NVMe");
        agregarAlCatalogo<SSD>(Ficha{"SSD-03", "SSD", "Samsung", 300, 36, 8, "500 GB:0.8|1 TB:1*|2 TB:1.9"}, 4, 1000, 3500, 3300, "M.2 NVMe");
        agregarAlCatalogo<HDD>(Ficha{"HDD-02", "HDD", "WD", 200, 24, 9, "1 TB:1|2 TB:1.5*|4 TB:2.4"}, 8, 2000, 180, 180, "3.5\"");
        agregarAlCatalogo<HDD>(Ficha{"HDD-01", "HDD", "Seagate", 190, 24, 14, "1 TB:1|2 TB:1.5*|4 TB:2.4"}, 8, 2000, 190, 190, "3.5\"");
        agregarAlCatalogo<PlacaMadre>(Ficha{"MB-01", "Motherboard", "Asus", 520, 24, 7, "B660:0.7|B760:1*|Z790:1.8"}, 45, "LGA1700", "B760", 4);
        agregarAlCatalogo<PlacaMadre>(Ficha{"MB-02", "Motherboard", "Gigabyte", 480, 24, 6, "A620:0.6|B650:1*|X670:1.9"}, 40, "AM5", "B650", 4);
        agregarAlCatalogo<FuentePoder>(Ficha{"PSU-01", "Fuente de poder", "Corsair", 280, 36, 9, "550 W:0.8|650 W:1*|850 W:1.6"}, 650, "80 Plus Bronze");
        agregarAlCatalogo<Ventilador>(Ficha{"FAN-01", "Ventilador", "CoolerMaster", 45, 12, 25}, 4, 1800, 120);

        // Accesorios
        agregarAlCatalogo<MemoriaFlash>(Ficha{"USB-01", "Memoria USB", "Kingston", 35, 12, 40, "32 GB:0.7|64 GB:1*|128 GB:1.8"}, "Universal", 64, "USB");
        agregarAlCatalogo<MemoriaFlash>(Ficha{"SD-01", "Memoria SD", "SanDisk", 40, 12, 30, "32 GB:0.7|64 GB:1*|128 GB:1.8"}, "Cámaras y celulares", 64, "SD");
        agregarAlCatalogo<PastaTermica>(Ficha{"TIM-01", "Pasta termica", "Arctic", 15, 6, 50, "4 g:0.8|8 g:1*|30 g:2.5"}, "CPU y GPU", 8);
        agregarAlCatalogo<Cable>(Ficha{"CAB-01", "Cable de poder", "Genérico", 20, 6, 35, "1.8 m:1*|3 m:1.5"}, "Fuentes ATX", "C13", 1.8);
        agregarAlCatalogo<Adaptador>(Ficha{"ADP-01", "HDMI a VGA", "Ugreen", 30, 12, 22, "Estándar:1*|Con audio:1.4"}, "Monitores y proyectores", "HDMI", "VGA");
        agregarAlCatalogo<Cargador>(Ficha{"CHG-01", "Cargador tipo C", "Anker", 55, 12, 18, "20 W:0.7|45 W:1*|65 W:1.5"}, "Celulares y laptops", "USB-C", 45);
        agregarAlCatalogo<Cable>(Ficha{"SATA-01", "Extension SATA", "Genérico", 12, 3, 40, "0.5 m:1*|1 m:1.4"}, "Discos SATA", "SATA", 0.5);

        // Periféricos
        agregarAlCatalogo<Mouse>(Ficha{"MOU-01", "Mouse inalambrico", "Logitech", 50, 12, 25, "Básico:0.8|Ergonómico:1*|Gamer RGB:1.8"}, "USB 2.4 GHz", true, false, 1600, false);
        agregarAlCatalogo<Mouse>(Ficha{"MOU-02", "Mouse con cable", "Redragon", 25, 12, 30, "Básico:0.8|Estándar:1*|Gamer:1.7"}, "USB", false, false, 3200, true);
        agregarAlCatalogo<Teclado>(Ficha{"TEC-01", "Teclado", "Redragon", 70, 12, 18, "Membrana:0.8|Mecánico:1.4*|Mecánico RGB:2"}, "USB", false, false, "Mecánico", true);
        agregarAlCatalogo<Parlante>(Ficha{"PAR-01", "Parlante", "JBL", 90, 12, 12, "Mini:0.7|Estándar:1*|Potente:1.8"}, "Bluetooth", true, true, 20);
        agregarAlCatalogo<CamaraWeb>(Ficha{"CAM-01", "Camara web", "Logitech", 120, 12, 10, "720p:0.7|1080p:1*|4K:2"}, "USB", false, false, 30, "1080p");
        agregarAlCatalogo<Microfono>(Ficha{"MIC-01", "Microfono", "HyperX", 110, 12, 9, "Estándar:0.8|Condensador:1*|Pro:1.8"}, "USB", false, false, "Condensador");
        agregarAlCatalogo<AdaptadorInalambrico>(Ficha{"WIFI-01", "Adaptador WiFi", "TP-Link", 40, 12, 20, "WiFi 5:0.8|WiFi 6:1*"}, "USB", true, true, "WiFi + Bluetooth 5.0");

        // Software
        agregarAlCatalogo<SistemaOperativo>(Ficha{"OS-01", "Windows (OS)", "Microsoft", 450, 0, 99, "Home:0.7|Pro:1*|Enterprise:1.6"}, true, 0, "64 bits");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-01", "Adobe", "Adobe", 300, 0, 99, "1 mes:0.2|12 meses:1*|24 meses:1.8"}, false, 12, "Diseño y edición");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-02", "Avast", "Avast", 90, 0, 99, "1 PC:0.6|3 PC:1*|5 PC:1.4"}, false, 12, "Antivirus");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-03", "Office", "Microsoft", 380, 0, 99, "Hogar y Estudiante:0.8|Hogar y Empresas:1*|Profesional:1.5"}, true, 0, "Ofimática");

        // Equipos completos
        agregarAlCatalogo<Computadora>(Ficha{"PC-01", "Computadora", "HP", 2500, 12, 5, "Oficina:0.7|Hogar:1*|Gamer:1.9"}, "Core i5", "16 GB", "512 GB SSD", "ATX");
        agregarAlCatalogo<Laptop>(Ficha{"LAP-01", "Laptop", "Lenovo", 3200, 12, 6, "14 pulg Básica:0.7|15.6 pulg Estándar:1*|16 pulg Pro:1.7"}, "Ryzen 5", "16 GB", "512 GB SSD", 15.6, 60, false);
        agregarAlCatalogo<Impresora>(Ficha{"PRN-01", "Impresora", "Epson", 600, 6, 8, "Inyección de tinta:0.7|Multifuncional:1*|Láser:1.6"}, "Tinta continua", 15, true);

        // Servicios
        agregarAlCatalogo<Mantenimiento>(Ficha{"SRV-01", "Mantenimiento", "TecnoStore", 80, 0, 99, "Básico:0.7|Completo:1*|Premium:1.6"}, "Limpieza y optimización", 3);
        agregarAlCatalogo<Garantia>(Ficha{"SRV-02", "Garantia", "TecnoStore", 150, 0, 99, "6 meses:0.6|12 meses:1*|24 meses:1.8"}, "Extensión de garantía", "gratuito");
        agregarAlCatalogo<Servicio>(Ficha{"SRV-03", "Armado de PC", "TecnoStore", 100, 0, 99, "Estándar:1*|Con pruebas:1.4"}, "Armado, cableado y pruebas");

        // Ofertas y listas ampliadas de modelos
        for (auto& p : catalogo_) {
            if (p->nombre() == "Mouse inalambrico" || p->nombre() == "Teclado")
                p->marcarOferta();
            auto it = modelosAmpliados().find(p->codigo());
            if (it != modelosAmpliados().end())
                p->fijarModelos(it->second);
        }
    }

    void crearOpcionesDeCompra() {
        mediosPago_.push_back(make_unique<Efectivo>());
        mediosPago_.push_back(make_unique<Tarjeta>());
        mediosPago_.push_back(make_unique<Yape>());
        mediosPago_.push_back(make_unique<Plin>());
        comprobantes_.push_back(make_unique<Boleta>());
        comprobantes_.push_back(make_unique<Factura>());
        envios_.push_back(make_unique<RecojoEnTienda>());
        envios_.push_back(make_unique<Delivery>());
    }

    void crearPersonal() {
        equipo_.push_back(make_unique<Gerente>(DatosPersona{"Rosa", "Mamani", "Quispe", "951000001", "rosa@tecnostore.pe", "40111111"}, "G-001", 3500, 0.5));
        equipo_.push_back(make_unique<Vendedor>(DatosPersona{"Luis", "Condori", "Flores", "951000002", "luis@tecnostore.pe", "40222222"}, "V-001", 1500, 3.0));
        equipo_.push_back(make_unique<Vendedor>(DatosPersona{"Ana", "Huanca", "Ccori", "951000003", "ana@tecnostore.pe", "40333333"}, "V-002", 1500, 3.5));
        equipo_.push_back(make_unique<Cajero>(DatosPersona{"Carlos", "Apaza", "Ramos", "951000004", "carlos@tecnostore.pe", "40444444"}, "C-001", 1200, "08:00", "16:00"));
        equipo_.push_back(make_unique<Tecnico>(DatosPersona{"Diego", "Choque", "Pari", "951000005", "diego@tecnostore.pe", "40555555"}, "T-001", 1800, "Hardware y redes"));
        proveedores_.push_back(make_unique<Proveedor>(DatosPersona{"Mario", "Gomez", "Lazo", "952000001", "ventas@andina.pe", "41111111"}, "Distribuidora Andina SAC", "20123456789", "Nacional", "Perú"));
        proveedores_.push_back(make_unique<Proveedor>(DatosPersona{"Li", "Wei", "Chen", "952000002", "sales@globaltech.com", "41222222"}, "Global Tech Import", "20987654321", "Internacional", "China"));
    }

    void crearPromocionesYCupones() {
        carrito_.agregarPromo(make_unique<PromoCombo>("Mouse inalambrico", "Teclado", 10.0));
        carrito_.agregarPromo(make_unique<PromoVolumen>(3, 0.05));
        cupones_.push_back(make_unique<CuponPorcentaje>("TECNO10", "10% de descuento", 0, 10));
        cupones_.push_back(make_unique<CuponMonto>("BIENVENIDO", "S/ 20 de descuento", 100, 20));
        cupones_.push_back(make_unique<CuponMonto>("GAMER50", "S/ 50 de descuento", 1000, 50));
        cupones_.push_back(make_unique<CuponPorcentaje>("PRIMERA5", "5% de descuento", 0, 5));
        cupones_.push_back(make_unique<CuponPorcentaje>("ACEITUNO", "90% de descuento", 0, 90));
    }

public:
    Tienda() {
        crearCatalogoBase();
        crearOpcionesDeCompra();
        crearPersonal();
        crearPromocionesYCupones();
        cargarInventario();  // aplica stock guardado y productos creados por el usuario
    }

    // ---------------- catálogo ----------------
    vector<const Producto*> productos() const {
        vector<const Producto*> resultado;
        for (auto& p : catalogo_)
            resultado.push_back(p.get());
        return resultado;
    }

    // Primer código libre con formato PRD-001, PRD-002...
    string siguienteCodigo() const {
        for (int n = 1; n < 10000; n++) {
            string codigo = TextFormat("PRD-%03d", n);
            bool usado = false;
            for (const auto& p : catalogo_)
                if (p->codigo() == codigo) {
                    usado = true;
                    break;
                }
            if (!usado)
                return codigo;
        }
        return "PRD-NUEVO";
    }

    bool agregarPersonalizado(Ficha ficha, int categoria, string detalle) {
        if (ficha.nombre.empty() || ficha.marca.empty() || ficha.precio < 0 || ficha.stock < 0)
            return false;
        for (const auto& p : catalogo_)
            if (p->codigo() == ficha.codigo)
                return false;
        catalogo_.push_back(make_unique<ProductoPersonalizado>(ficha, Clamp(categoria, 0, 5), move(detalle)));
        guardarInventario();
        return true;
    }

    bool reponerStock(const string& codigo, int cantidad) {
        if (cantidad <= 0)
            return false;
        for (auto& p : catalogo_)
            if (p->codigo() == codigo) {
                p->fijarStock(p->stock() + cantidad);
                guardarInventario();
                return true;
            }
        return false;
    }

    vector<string> marcas() const {  // marcas sin repetir, en orden de aparición
        vector<string> resultado;
        for (auto& p : catalogo_)
            if (find(resultado.begin(), resultado.end(), p->marca()) == resultado.end())
                resultado.push_back(p->marca());
        return resultado;
    }

    // ---------------- personal ----------------
    vector<const Vendedor*> vendedores() const {
        vector<const Vendedor*> resultado;
        for (auto& trabajador : equipo_)
            if (auto vendedor = dynamic_cast<const Vendedor*>(trabajador.get()))
                resultado.push_back(vendedor);
        return resultado;
    }

    const vector<unique_ptr<Trabajador>>& equipo() const {
        return equipo_;
    }

    const vector<unique_ptr<Proveedor>>& proveedores() const {
        return proveedores_;
    }

    const vector<Pedido>& pedidos() const {
        return pedidos_;
    }

    Carrito& carrito() {
        return carrito_;
    }

    // ---------------- opciones de compra ----------------
    vector<string> nombresMedios() const {
        return nombresDe(mediosPago_);
    }

    vector<string> nombresComprobantes() const {
        return nombresDe(comprobantes_);
    }

    vector<string> nombresEnvios() const {
        return nombresDe(envios_);
    }

    int medio() const {
        return medioElegido_;
    }

    int comprobante() const {
        return comprobanteElegido_;
    }

    int envio() const {
        return envioElegido_;
    }

    void elegirMedio(int i) {
        medioElegido_ = i;
    }

    void elegirComprobante(int i) {
        comprobanteElegido_ = i;
    }

    void elegirEnvio(int i) {
        envioElegido_ = i;
    }

    const MedioPago& medioActual() const {
        return *mediosPago_[medioElegido_];
    }

    const Comprobante& comprobanteActual() const {
        return *comprobantes_[comprobanteElegido_];
    }

    const Envio& envioActual() const {
        return *envios_[envioElegido_];
    }

    // ---------------- cupones ----------------
    // Devuelve "" si el cupón se aplicó; si no, el motivo del rechazo.
    string aplicarCupon(const string& texto) {
        string codigo = mayusc(texto);
        if (codigo.empty())
            return "Escribe un código";
        for (auto& cupon : cupones_)
            if (cupon->codigo() == codigo) {
                double netoSinCupon = carrito_.subtotal() - carrito_.descuentos();
                if (netoSinCupon + 1e-9 < cupon->minimo())
                    return "Compra mínima " + dinero(cupon->minimo());
                cuponAplicado_ = cupon.get();
                return "";
            }
        return "Cupón no válido";
    }

    void quitarCupon() {
        cuponAplicado_ = nullptr;
    }

    const Cupon* cupon() const {
        return cuponAplicado_;
    }

    // ---------------- totales ----------------
    double descuentoCupon() const {
        if (!cuponAplicado_ || carrito_.cantidad() == 0)
            return 0;
        return cuponAplicado_->descuento(carrito_.subtotal() - carrito_.descuentos());
    }

    // Total antes del recargo del medio de pago: subtotal - promociones - cupón + envío
    double base() const {
        double envio = carrito_.cantidad() ? envios_[envioElegido_]->costo() : 0;
        return carrito_.subtotal() - carrito_.descuentos() - descuentoCupon() + envio;
    }

    double recargo() const {
        return carrito_.cantidad() ? mediosPago_[medioElegido_]->recargo(base()) : 0;
    }

    double total() const {
        return base() + recargo();
    }

    // ---------------- pagar ----------------
    // Registra el pedido, descuenta stock, vacía el carrito y guarda el inventario.
    Pedido pagar(const Cliente& cliente, const string& ruc, const string& razonSocial, const string& direccion,
                 const string& datoPago, int indiceVendedor) {
        Pedido pedido;
        pedido.total = total();
        pedido.cupon = cuponAplicado_ ? cuponAplicado_->codigo() : string();

        const Comprobante& comprobante = *comprobantes_[comprobanteElegido_];
        const Envio& envio = *envios_[envioElegido_];

        time_t ahora = time(nullptr);
        tm* fecha = localtime(&ahora);
        pedido.numero = comprobante.serie() + "-" + TextFormat("%06d", ++contadorPedidos_);
        pedido.fecha = TextFormat("%02d/%02d/%04d", fecha->tm_mday, fecha->tm_mon + 1, fecha->tm_year + 1900);
        pedido.cliente = cliente.nombreCompleto();
        pedido.documento = comprobante.pideRUC() ? "RUC " + ruc + " · " + razonSocial : "DNI " + cliente.datos().dni;
        pedido.entrega = envio.nombre() + (envio.pideDireccion() ? " · " + direccion : string());

        const Vendedor* vendedor = vendedores()[(size_t)indiceVendedor];
        pedido.vendedor = vendedor->nombreCompleto();
        pedido.comision = vendedor->comision(pedido.total);
        pedido.comprobante = comprobante.emitir(pedido.total);
        pedido.pago = mediosPago_[medioElegido_]->resumen(datoPago, pedido.total);

        for (auto& linea : carrito_.lineas()) {
            pedido.items.push_back(TextFormat("%d× %s %s", linea.cantidad, linea.producto->nombre().c_str(),
                                              linea.producto->modelos()[(size_t)linea.modelo].nombre.c_str()));
            for (auto& p : catalogo_)
                if (p.get() == linea.producto)
                    p->quitarStock(linea.cantidad);
        }

        guardarInventario();
        carrito_.vaciar();
        cuponAplicado_ = nullptr;
        pedidos_.push_back(pedido);
        return pedido;
    }
};

// =====================================================================================
// 6. INTERFAZ: ayudas de dibujo (raylib) - estilo "vidrio"
// =====================================================================================

// ---- Paleta de colores ----
static const Color C_BG{247, 248, 252, 255};         // fondo
static const Color C_TINTA{31, 39, 97, 255};         // texto principal
static const Color C_MUTED{108, 116, 156, 255};      // texto secundario
static const Color C_SUAVE{234, 241, 253, 255};      // fondo suave / hover
static const Color C_LINEA{201, 220, 250, 255};      // bordes
static const Color C_VIDRIO{255, 255, 255, 170};     // paneles translúcidos
static const Color C_MODAL{255, 255, 255, 255};      // ventanas emergentes
static const Color C_OK{22, 163, 98, 255};           // verde
static const Color C_PELIGRO{225, 64, 64, 255};      // rojo
static const Color C_PRIM{16, 28, 120, 255};         // azul marino (color principal)
static const Color C_ACENTO{245, 166, 35, 255};      // naranja
static const Color C_AVISO{217, 119, 6, 255};        // ámbar

static Font FUENTE;
static unordered_map<string, Texture2D> TEXTURAS;  // caché de imágenes ya cargadas

static Rectangle R(float x, float y, float ancho, float alto) {
    return Rectangle{x, y, ancho, alto};
}

// Busca una fuente del sistema con soporte para tildes y ñ (ASCII + Latin-1)
static void cargarFuente() {
    const char* rutas[] = {"C:/Windows/Fonts/segoeui.ttf",
                           "C:/Windows/Fonts/arial.ttf",
                           "/System/Library/Fonts/Supplemental/Arial.ttf",
                           "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                           "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                           "/usr/share/fonts/TTF/DejaVuSans.ttf",
                           "/usr/share/fonts/noto/NotoSans-Regular.ttf"};
    static int codepoints[224];
    for (int i = 0; i < 224; i++)
        codepoints[i] = 32 + i;

    FUENTE = GetFontDefault();
    for (const char* ruta : rutas) {
        if (!FileExists(ruta))
            continue;
        Font fuente = LoadFontEx(ruta, 48, codepoints, 224);
        if (fuente.texture.id != 0) {
            SetTextureFilter(fuente.texture, TEXTURE_FILTER_BILINEAR);
            FUENTE = fuente;
            break;
        }
    }
}

// Carga perezosa de imágenes; devuelve nullptr si el archivo no existe
static const Texture2D* textura(const string& ruta) {
    auto it = TEXTURAS.find(ruta);
    if (it == TEXTURAS.end()) {
        Texture2D t{};
        if (FileExists(ruta.c_str())) {
            t = LoadTexture(ruta.c_str());
            if (t.id)
                SetTextureFilter(t, TEXTURE_FILTER_BILINEAR);
        }
        it = TEXTURAS.emplace(ruta, t).first;
    }
    return it->second.id ? &it->second : nullptr;
}

// Mezcla dos colores (t = 0 -> a, t = 1 -> b)
static Color Mezcla(Color a, Color b, float t) {
    return Color{(unsigned char)(a.r + (b.r - a.r) * t), (unsigned char)(a.g + (b.g - a.g) * t),
                 (unsigned char)(a.b + (b.b - a.b) * t), (unsigned char)(a.a + (b.a - a.a) * t)};
}

// Aplica transparencia a un color (alfa 0..1)
static Color Al(Color color, float alfa) {
    color.a = (unsigned char)(color.a * Clamp(alfa, 0.0f, 1.0f));
    return color;
}

// Dibuja una imagen centrada dentro del área, sin deformarla. Devuelve false si no existe.
static bool imagen(const string& ruta, Rectangle area, float alfa = 1.0f) {
    const Texture2D* tex = textura(ruta);
    if (!tex)
        return false;
    float escala = min(area.width / tex->width, area.height / tex->height);
    float w = tex->width * escala, h = tex->height * escala;
    DrawTexturePro(*tex, R(0, 0, (float)tex->width, (float)tex->height),
                   R(area.x + (area.width - w) / 2, area.y + (area.height - h) / 2, w, h), Vector2{0, 0}, 0, Al(WHITE, alfa));
    return true;
}

// ---- Texto ----
static void Texto(const string& s, float x, float y, float tam, Color color, float espaciado = 0.5f) {
    DrawTextEx(FUENTE, s.c_str(), Vector2{x, y}, tam, espaciado, color);
}

static float Ancho(const string& s, float tam, float espaciado = 0.5f) {
    return MeasureTextEx(FUENTE, s.c_str(), tam, espaciado).x;
}

static void TextoCentro(const string& s, Rectangle r, float tam, Color color) {
    Texto(s, r.x + (r.width - Ancho(s, tam)) / 2, r.y + (r.height - tam) / 2 - 1, tam, color);
}

static void TextoDer(const string& s, float xDerecha, float y, float tam, Color color) {
    Texto(s, xDerecha - Ancho(s, tam), y, tam, color);
}

// Recorta el texto con ".." si no cabe en el ancho máximo (respeta caracteres UTF-8)
static string Ajustar(string s, float tam, float anchoMax) {
    if (Ancho(s, tam) <= anchoMax)
        return s;
    while (!s.empty() && Ancho(s + "..", tam) > anchoMax) {
        while (!s.empty() && ((unsigned char)s.back() & 0xC0) == 0x80)
            s.pop_back();
        if (!s.empty())
            s.pop_back();
    }
    return s + "..";
}

// ---- Envoltorios para funciones de raylib que cambiaron en la versión 5.5 ----
static void LineasR(Rectangle r, float redondeo, int segmentos, float grosor, Color color) {
#if defined(RAYLIB_VERSION_MAJOR) && (RAYLIB_VERSION_MAJOR * 100 + RAYLIB_VERSION_MINOR >= 505)
    DrawRectangleRoundedLinesEx(r, redondeo, segmentos, grosor, color);
#else
    DrawRectangleRoundedLines(r, redondeo, segmentos, grosor, color);
#endif
}

static void CirculoDeg(Vector2 centro, float radio, Color interior, Color exterior) {
#if defined(RAYLIB_VERSION_MAJOR) && (RAYLIB_VERSION_MAJOR * 100 + RAYLIB_VERSION_MINOR >= 505)
    DrawCircleGradient(centro, radio, interior, exterior);
#else
    DrawCircleGradient((int)centro.x, (int)centro.y, radio, interior, exterior);
#endif
}

// ---- Figuras con estilo ----
static void Caja(Rectangle r, float redondeo, Color fondo, Color borde) {
    DrawRectangleRounded(r, redondeo, 12, fondo);
    LineasR(r, redondeo, 12, 1, borde);
}

static void Sombra(Rectangle r, float redondeo, float intensidad) {
    for (int i = 3; i >= 1; i--)
        DrawRectangleRounded(R(r.x - i * 2.0f, r.y + i * 3.0f - 1, r.width + i * 4.0f, r.height + i * 4.0f), redondeo, 10,
                             Al(Color{60, 80, 150, 255}, 0.035f * intensidad));
}

// Panel blanco translúcido con borde y brillo superior
static void Vidrio(Rectangle r, float redondeo, float intensidadSombra = 1.0f, Color tinte = C_VIDRIO) {
    Sombra(r, redondeo, intensidadSombra);
    DrawRectangleRounded(r, redondeo, 12, tinte);
    LineasR(r, redondeo, 12, 1, Color{200, 210, 236, 170});
    float radio = redondeo * min(r.width, r.height) / 2;
    DrawRectangle((int)(r.x + radio), (int)r.y + 1, (int)(r.width - 2 * radio), 1, WHITE);
}

// Degradado con esquinas redondeadas (horizontal = true: de izquierda a derecha)
static void Deg(Rectangle r, float radio, Color colorA, Color colorB, bool horizontal) {
    float tapa = radio * 2 + 8;
    Rectangle tapaA = horizontal ? R(r.x, r.y, tapa, r.height) : R(r.x, r.y, r.width, tapa);
    Rectangle tapaB = horizontal ? R(r.x + r.width - tapa, r.y, tapa, r.height) : R(r.x, r.y + r.height - tapa, r.width, tapa);
    float redondeo = radio * 2 / min(tapaA.width, tapaA.height);
    DrawRectangleRounded(tapaA, redondeo, 12, colorA);
    DrawRectangleRounded(tapaB, redondeo, 12, colorB);
    if (horizontal)
        DrawRectangleGradientH((int)(r.x + tapa / 2), (int)r.y, (int)(r.width - tapa), (int)r.height, colorA, colorB);
    else
        DrawRectangleGradientV((int)r.x, (int)(r.y + tapa / 2), (int)r.width, (int)(r.height - tapa), colorA, colorB);
}

// Cada marca sin logo recibe un color propio, calculado a partir de su nombre
static Color colorMarca(const string& marca) {
    return ColorFromHSV((float)(hash<string>{}(marca) % 360), 0.50f, 0.78f);
}

// Etiqueta de marca: usa img/marcas/<marca>.png si existe; si no, un recuadro de color con el nombre
static void Marca(const string& marca, Rectangle r, float alfa = 1.0f) {
    string rutaLogo = "img/marcas/" + marca + ".png";
    if (textura(rutaLogo)) {
        DrawRectangleRounded(r, 0.3f, 8, Al(WHITE, 0.92f * alfa));
        LineasR(r, 0.3f, 8, 1, Al(C_LINEA, alfa));
        imagen(rutaLogo, R(r.x + 6, r.y + 4, r.width - 12, r.height - 8), alfa);
        return;
    }
    DrawRectangleRounded(r, 0.3f, 8, Al(colorMarca(marca), alfa));
    float tam = r.height * 0.45f, ancho = Ancho(marca, tam);
    if (ancho > r.width - 10)
        tam *= (r.width - 10) / ancho;
    TextoCentro(marca, r, tam, Al(WHITE, alfa));
}

// =====================================================================================
// 7. INTERFAZ: ilustraciones de productos
//    Si no existe img/productos/<codigo>.png, se dibuja una ilustración simple del producto.
// =====================================================================================

// ---- Clasificación: a qué ilustración corresponde cada producto ----
// {palabra clave, ¿solo palabra completa?, tipo de dibujo}. EL ORDEN IMPORTA: gana la primera coincidencia.
struct Regla {
    const char* palabraClave;
    bool palabraCompleta;
    const char* tipo;
};

static const Regla REGLAS[] = {{"wifi", 0, "wifi"},
                               {"wi-fi", 0, "wifi"},
                               {"armado", 0, "armado"},
                               {"mantenimiento", 0, "mant"},
                               {"limpieza", 0, "mant"},
                               {"garantia", 0, "garantia"},
                               {"garantía", 0, "garantia"},
                               {"gpu", 1, "gpu"},
                               {"video", 0, "gpu"},
                               {"grafic", 0, "gpu"},
                               {"cpu", 1, "cpu"},
                               {"procesador", 0, "cpu"},
                               {"m.2", 0, "m2"},
                               {"nvme", 0, "m2"},
                               {"ssd", 1, "ssd"},
                               {"hdd", 1, "hdd"},
                               {"disco", 0, "hdd"},
                               {"motherboard", 0, "placa"},
                               {"placa", 0, "placa"},
                               {"fuente", 0, "psu"},
                               {"psu", 1, "psu"},
                               {"ventilador", 0, "fan"},
                               {"cooler", 0, "fan"},
                               {"ram", 1, "ram"},
                               {"usb", 1, "usb"},
                               {"pendrive", 0, "usb"},
                               {"sd", 1, "sd"},
                               {"pasta", 0, "pasta"},
                               {"mouse", 0, "mouse"},
                               {"raton", 0, "mouse"},
                               {"teclado", 0, "teclado"},
                               {"keyboard", 0, "teclado"},
                               {"parlante", 0, "parlante"},
                               {"altavoz", 0, "parlante"},
                               {"speaker", 0, "parlante"},
                               {"camara", 0, "camara"},
                               {"webcam", 0, "camara"},
                               {"microfono", 0, "mic"},
                               {"mic", 1, "mic"},
                               {"hdmi", 0, "adaptador"},
                               {"vga", 0, "adaptador"},
                               {"adaptador", 0, "adaptador"},
                               {"cable", 0, "cable"},
                               {"extension", 0, "cable"},
                               {"sata", 1, "cable"},
                               {"cargador", 0, "cargador"},
                               {"windows", 0, "windows"},
                               {"sistema operativo", 0, "windows"},
                               {"adobe", 0, "adobe"},
                               {"avast", 0, "avast"},
                               {"antivirus", 0, "avast"},
                               {"office", 0, "office"},
                               {"laptop", 0, "laptop"},
                               {"notebook", 0, "laptop"},
                               {"monitor", 0, "monitor"},
                               {"pantalla", 0, "monitor"},
                               {"impresora", 0, "impresora"},
                               {"computadora", 0, "pc"},
                               {"pc", 1, "pc"},
                               {"gabinete", 0, "pc"}};

// ¿El texto contiene la palabra completa (sin letras pegadas a los lados)?
static bool contienePalabra(const string& texto, const string& palabra) {
    for (size_t i = 0; (i = texto.find(palabra, i)) != string::npos; i++) {
        size_t fin = i + palabra.size();
        bool inicioLimpio = i == 0 || !isalpha((unsigned char)texto[i - 1]);
        bool finLimpio = fin >= texto.size() || !isalpha((unsigned char)texto[fin]);
        if (inicioLimpio && finLimpio)
            return true;
    }
    return false;
}

static string clasificar(const string& texto) {
    for (auto& regla : REGLAS) {
        bool coincide = regla.palabraCompleta ? contienePalabra(texto, regla.palabraClave)
                                              : texto.find(regla.palabraClave) != string::npos;
        if (coincide)
            return regla.tipo;
    }
    return "";
}

// ---- Colores propios de las ilustraciones ----
namespace Ilus {
static const Color tinta{53, 66, 120, 255}, brillo{88, 112, 190, 255}, claro{221, 230, 250, 255}, oro{245, 190, 60, 255},
    oscuro{34, 44, 92, 255}, blanco{255, 255, 255, 255}, metal{170, 180, 200, 255}, celeste{190, 205, 240, 255};
}  // namespace Ilus

// ---- Lienzo: herramientas de dibujo con coordenadas RELATIVAS AL CENTRO y escaladas ----
// Todas las medidas (x, y, ancho, alto) se dan respecto al centro del área; "escala" las adapta al tamaño real.
struct Lienzo {
    Rectangle area;
    Vector2 centro;
    float escala;
    Color acento;

    Lienzo(Rectangle a, Color ac)
        : area(a), centro{a.x + a.width / 2, a.y + a.height / 2}, escala(min(a.width / 300.0f, a.height / 180.0f)), acento(ac) {
    }

    // Rectángulo redondeado
    void rect(float x, float y, float ancho, float alto, float redondeo, Color color) const {
        DrawRectangleRounded(R(centro.x + x * escala, centro.y + y * escala, ancho * escala, alto * escala), redondeo, 8, color);
    }

    void circulo(float x, float y, float radio, Color color) const {
        DrawCircleV(Vector2{centro.x + x * escala, centro.y + y * escala}, radio * escala, color);
    }

    void aro(float x, float y, float radio, Color color) const {  // circunferencia (solo borde)
        DrawCircleLinesV(Vector2{centro.x + x * escala, centro.y + y * escala}, radio * escala, color);
    }

    void linea(float x1, float y1, float x2, float y2, float grosor, Color color) const {
        DrawLineEx(Vector2{centro.x + x1 * escala, centro.y + y1 * escala}, Vector2{centro.x + x2 * escala, centro.y + y2 * escala},
                   grosor * escala, color);
    }

    // Texto centrado dentro de un rectángulo relativo
    void texto(const string& t, float x, float y, float ancho, float alto, float tam, Color color) const {
        TextoCentro(t, R(centro.x + x * escala, centro.y + y * escala, ancho * escala, alto * escala), tam * escala, color);
    }

    // Aspas que giran con el tiempo (ventiladores)
    void aspas(float x, float y, float radio, int cantidad, Color color, float grosor) const {
        float rotacion = (float)GetTime() * 2.5f;
        for (int i = 0; i < cantidad; i++) {
            float angulo = rotacion + i * 2 * PI / cantidad;
            linea(x + cosf(angulo) * 6, y + sinf(angulo) * 6, x + cosf(angulo + 0.35f) * radio, y + sinf(angulo + 0.35f) * radio,
                  grosor, color);
        }
    }

    // Caja de software con lomo lateral
    void cajaSoftware(Color color) const {
        rect(-48, -66, 96, 132, 0.06f, color);
        rect(-48, -66, 14, 132, 0.4f, Mezcla(color, Color{0, 0, 0, 255}, 0.25f));
    }
};

// ---- Una función de dibujo por tipo de producto ----
static void dibujarGPU(const Lienzo& L) {
    using namespace Ilus;
    for (int i = 0; i < 10; i++)
        L.rect(-60 + i * 9.0f, 28, 5, 11, 0.2f, oro);  // contactos dorados
    L.rect(-102, -40, 8, 80, 0.3f, claro);             // bracket
    L.rect(-95, -34, 190, 64, 0.1f, tinta);            // cuerpo
    L.rect(-95, -34, 190, 8, 0.5f, brillo);
    for (float xVentilador : {-48.0f, 18.0f}) {
        L.circulo(xVentilador, 0, 27, oscuro);
        L.aspas(xVentilador, 0, 23, 7, brillo, 9);
        L.circulo(xVentilador, 0, 6, claro);
        L.aro(xVentilador, 0, 27, claro);
    }
    L.rect(60, -42, 26, 8, 0.3f, oro);
    L.texto("GPU", 50, -8, 44, 20, 16, blanco);
}

static void dibujarCPU(const Lienzo& L) {
    using namespace Ilus;
    for (int i = 0; i < 8; i++) {  // pines en los 4 lados
        L.rect(-42 + i * 12.0f, -57, 5, 12, 0.2f, oro);
        L.rect(-42 + i * 12.0f, 45, 5, 12, 0.2f, oro);
        L.rect(-57, -42 + i * 12.0f, 12, 5, 0.2f, oro);
        L.rect(45, -42 + i * 12.0f, 12, 5, 0.2f, oro);
    }
    L.rect(-48, -48, 96, 96, 0.1f, claro);
    L.rect(-34, -34, 68, 68, 0.08f, tinta);
    DrawTriangle(Vector2{L.centro.x - 44 * L.escala, L.centro.y - 44 * L.escala},
                 Vector2{L.centro.x - 44 * L.escala, L.centro.y - 32 * L.escala},
                 Vector2{L.centro.x - 32 * L.escala, L.centro.y - 44 * L.escala}, oro);  // marca del pin 1
    L.texto("CPU", -34, -34, 68, 68, 22, blanco);
}

static void dibujarRAM(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-98, -28, 196, 56, 0.08f, tinta);
    L.rect(-98, -28, 196, 13, 0.5f, brillo);
    L.texto("RAM", -98, -28, 196, 13, 10, blanco);
    for (int i = 0; i < 6; i++) {  // chips
        L.rect(-86 + i * 28.0f, -8, 22, 26, 0.12f, oscuro);
        L.linea(-82 + i * 28.0f, -2, -68 + i * 28.0f, -2, 2, brillo);
    }
    for (int i = 0; i < 17; i++)  // contactos (con la muesca en el centro)
        if (i != 8)
            L.rect(-90 + i * 11.0f, 28, 7, 11, 0.1f, oro);
}

static void dibujarM2(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-98, -20, 196, 40, 0.18f, tinta);
    L.rect(-98, -14, 16, 28, 0.2f, oro);
    L.rect(-62, -13, 36, 26, 0.12f, oscuro);
    L.rect(-20, -13, 36, 26, 0.12f, oscuro);
    L.rect(24, -13, 32, 26, 0.12f, oscuro);
    L.texto("SSD", -20, -13, 36, 26, 11, blanco);
    L.circulo(86, 0, 6, claro);
    L.circulo(86, 0, 3, tinta);
}

static void dibujarSSD(const Lienzo& L) {
    using namespace Ilus;
    for (int i = 0; i < 7; i++)
        L.rect(-24 + i * 8.0f, 44, 5, 14, 0.2f, oro);
    L.rect(-46, -62, 92, 112, 0.08f, tinta);
    L.rect(-34, -50, 68, 58, 0.08f, claro);
    L.texto("SSD", -34, -48, 68, 36, 24, tinta);
    L.texto("SATA", -34, -16, 68, 20, 12, brillo);
    L.rect(-34, 18, 68, 6, 0.5f, L.acento);
    L.rect(-34, 32, 40, 5, 0.5f, brillo);
}

static void dibujarHDD(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-52, -62, 104, 124, 0.08f, tinta);
    L.rect(-47, -57, 94, 114, 0.06f, claro);
    L.circulo(0, -10, 38, tinta);  // plato
    L.circulo(0, -10, 34, Color{200, 210, 235, 255});
    L.aro(0, -10, 24, brillo);
    L.aro(0, -10, 16, brillo);
    L.circulo(0, -10, 8, claro);
    L.circulo(0, -10, 3, tinta);
    L.linea(34, 36, 6, -4, 5, tinta);  // brazo lector
    L.circulo(34, 36, 6, tinta);
    for (float x : {-41.0f, 41.0f})  // tornillos
        for (float y : {-51.0f, 51.0f})
            L.circulo(x, y, 3, brillo);
    L.rect(-26, 44, 52, 9, 0.3f, tinta);
    for (int i = 0; i < 6; i++)
        L.rect(-22 + i * 8.0f, 46, 4, 5, 0.2f, oro);
}

// Placa madre; si es "armado de PC" se le agrega un destornillador
static void dibujarPlaca(const Lienzo& L, bool conDestornillador) {
    using namespace Ilus;
    L.rect(-72, -68, 144, 136, 0.06f, tinta);
    L.rect(-56, -52, 50, 50, 0.08f, claro);  // socket
    L.rect(-48, -44, 34, 34, 0.06f, brillo);
    for (int i = 0; i < 4; i++)  // ranuras RAM
        L.rect(8 + i * 11.0f, -56, 6, 62, 0.3f, claro);
    L.rect(-56, 20, 104, 8, 0.3f, claro);  // ranuras PCIe
    L.rect(-56, 38, 70, 8, 0.3f, claro);
    L.rect(26, 40, 26, 22, 0.12f, brillo);
    for (int i = 0; i < 3; i++)
        L.circulo(56, 10 + i * 18.0f, 6, oro);
    L.linea(-6, -28, 8, -28, 2, brillo);
    L.linea(-30, -2, -30, 20, 2, brillo);
    if (conDestornillador) {
        L.linea(30, 52, 72, 6, 4, metal);
        L.linea(72, 6, 94, -22, 11, L.acento);
    }
}

static void dibujarFuente(const Lienzo& L) {
    using namespace Ilus;
    for (float y : {-18.0f, 2.0f, 22.0f})  // cables
        L.linea(72, y, 96, y, 6, oscuro);
    L.rect(92, -24, 9, 12, 0.2f, oro);
    L.rect(92, -4, 9, 12, 0.2f, oro);
    L.rect(-72, -48, 144, 96, 0.08f, tinta);
    L.circulo(-22, 0, 36, oscuro);
    L.aspas(-22, 0, 31, 7, brillo, 8);
    L.circulo(-22, 0, 7, claro);
    L.aro(-22, 0, 36, claro);
    L.texto("PSU", 18, -34, 50, 20, 15, blanco);
    L.linea(48, -6, 38, 10, 3, oro);  // rayo
    L.linea(38, 10, 52, 10, 3, oro);
    L.linea(52, 10, 42, 30, 3, oro);
}

static void dibujarVentilador(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-58, -58, 116, 116, 0.1f, tinta);
    L.circulo(0, 0, 50, oscuro);
    L.aspas(0, 0, 46, 7, brillo, 11);
    L.circulo(0, 0, 10, claro);
    L.circulo(0, 0, 4, tinta);
    for (float x : {-49.0f, 49.0f})
        for (float y : {-49.0f, 49.0f})
            L.circulo(x, y, 4, claro);
}

static void dibujarUSB(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-62, -24, 92, 48, 0.25f, tinta);
    L.rect(28, -16, 46, 32, 0.1f, claro);
    L.rect(34, -10, 36, 20, 0.1f, tinta);
    L.rect(36, -2, 30, 5, 0.4f, oro);
    L.circulo(-50, 0, 6, claro);
    L.texto("USB", -42, -14, 70, 24, 18, blanco);
    L.rect(-30, 14, 50, 5, 0.5f, L.acento);
}

static void dibujarSD(const Lienzo& L) {
    using namespace Ilus;
    for (int i = 0; i < 8; i++)
        L.rect(-27 + i * 7.5f, -52, 5, 16, 0.2f, oro);
    L.rect(-36, -50, 72, 100, 0.1f, tinta);
    L.rect(-38, -30, 6, 16, 0.4f, brillo);
    L.rect(-26, -16, 52, 52, 0.1f, claro);
    L.texto("SD", -26, -16, 52, 36, 26, tinta);
    L.rect(-18, 20, 36, 5, 0.5f, L.acento);
}

static void dibujarPasta(const Lienzo& L) {  // jeringa de pasta térmica
    using namespace Ilus;
    L.rect(-60, -15, 100, 30, 0.25f, tinta);
    L.rect(-57, -12, 94, 24, 0.25f, claro);
    L.rect(-57, -12, 62, 24, 0.25f, L.acento);
    for (int i = 0; i < 6; i++)
        L.linea(-20 + i * 11.0f, -12, -20 + i * 11.0f, -5, 2, tinta);
    L.rect(38, -22, 8, 44, 0.3f, tinta);
    L.rect(46, -5, 40, 10, 0.3f, brillo);
    L.rect(84, -16, 8, 32, 0.3f, tinta);
    L.rect(-84, -4, 26, 8, 0.5f, metal);
    L.rect(-96, -2, 14, 4, 0.5f, Color{140, 150, 175, 255});
    L.circulo(-92, 20, 9, L.acento);
    L.circulo(-92, 12, 4, L.acento);
}

// Mouse; con cable si la descripción lo menciona, con ondas si es inalámbrico
static void dibujarMouse(const Lienzo& L, const string& descripcion) {
    using namespace Ilus;
    bool inalambrico = descripcion.find("inal") != string::npos;
    bool conCable = descripcion.find("cable") != string::npos;
    if (conCable && !inalambrico) {
        L.linea(0, -56, 0, -68, 3, tinta);
        L.linea(0, -68, 40, -68, 3, tinta);
    }
    DrawEllipse((int)L.centro.x, (int)(L.centro.y + 3 * L.escala), (int)(43 * L.escala), (int)(57 * L.escala), tinta);
    DrawEllipseLines((int)L.centro.x, (int)(L.centro.y + 3 * L.escala), (int)(43 * L.escala), (int)(57 * L.escala), brillo);
    L.linea(0, -54, 0, -14, 3, brillo);
    L.linea(-40, -14, 40, -14, 2, brillo);
    L.rect(-5, -44, 10, 24, 0.5f, claro);
    L.rect(-18, 20, 36, 5, 0.5f, L.acento);
    if (inalambrico)
        for (int k = 0; k < 3; k++)
            DrawRing(Vector2{L.centro.x + 44 * L.escala, L.centro.y - 30 * L.escala}, (8 + k * 9) * L.escala,
                     (11 + k * 9) * L.escala, 270, 360, 16, brillo);
}

static void dibujarTeclado(const Lienzo& L) {
    using namespace Ilus;
    float e = L.escala;
    Rectangle base = R(L.centro.x - 91 * e, L.centro.y - 38 * e, 182 * e, 76 * e);
    DrawRectangleRounded(base, 0.12f, 10, tinta);
    for (int fila = 0; fila < 3; fila++)
        for (int col = 0; col < 12; col++)
            DrawRectangleRounded(R(base.x + (8 + col * 14) * e, base.y + (9 + fila * 18) * e, 10 * e, 11 * e), 0.18f, 3,
                                 (fila + col) % 5 == 0 ? brillo : claro);
    DrawRectangleRounded(R(base.x + 50 * e, base.y + 62 * e, 82 * e, 7 * e), 0.4f, 4, brillo);  // barra espaciadora
}

static void dibujarParlante(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-46, -64, 92, 128, 0.16f, tinta);
    L.circulo(0, -38, 13, oscuro);  // tweeter
    L.circulo(0, -38, 8, brillo);
    L.circulo(0, -38, 3, claro);
    L.circulo(0, 16, 34, oscuro);  // woofer
    L.aro(0, 16, 34, claro);
    L.circulo(0, 16, 24, brillo);
    L.circulo(0, 16, 10, oscuro);
    L.circulo(30, -54, 3, C_OK);
}

static void dibujarCamara(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-12, 28, 24, 26, 0.2f, tinta);
    L.rect(-44, 50, 88, 12, 0.4f, brillo);
    L.circulo(0, -12, 44, tinta);
    L.circulo(0, -12, 36, oscuro);
    L.circulo(0, -12, 28, brillo);
    L.circulo(0, -12, 18, tinta);
    L.circulo(-6, -18, 4, Al(WHITE, 0.8f));
    L.circulo(0, -50, 3, C_OK);
}

static void dibujarMicrofono(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-5, 14, 10, 40, 0.2f, tinta);
    L.rect(-36, 52, 72, 10, 0.4f, tinta);
    L.rect(-26, -72, 52, 84, 0.5f, tinta);
    for (int i = 0; i < 7; i++)
        L.linea(-19, -58 + i * 10.0f, 19, -58 + i * 10.0f, 2, brillo);
    L.rect(-30, 6, 60, 10, 0.4f, brillo);
}

static void dibujarWifi(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-70, -16, 80, 36, 0.2f, tinta);
    L.rect(10, -10, 42, 24, 0.1f, claro);
    L.rect(14, -5, 34, 14, 0.1f, tinta);
    L.circulo(-58, 2, 3, C_OK);
    L.texto("WiFi", -66, -12, 70, 28, 14, blanco);
    for (int k = 0; k < 3; k++)  // ondas
        DrawRing(Vector2{L.centro.x - 30 * L.escala, L.centro.y - 22 * L.escala}, (10 + k * 10) * L.escala,
                 (14 + k * 10) * L.escala, 225, 315, 16, k == 0 ? L.acento : brillo);
    L.circulo(-30, -22, 4, L.acento);
}

static void dibujarAdaptador(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-98, -16, 46, 32, 0.15f, claro);
    L.texto("HDMI", -98, -16, 46, 32, 11, tinta);
    L.rect(-52, -20, 58, 40, 0.15f, tinta);
    L.rect(6, -20, 74, 40, 0.12f, brillo);
    for (int i = 0; i < 5; i++)  // pines VGA
        L.circulo(22 + i * 11.0f, -6, 2.5f, claro);
    for (int i = 0; i < 4; i++)
        L.circulo(27 + i * 11.0f, 7, 2.5f, claro);
    L.linea(80, -8, 96, -8, 3, metal);
    L.linea(80, 8, 96, 8, 3, metal);
}

static void dibujarCable(const Lienzo& L) {  // cable enrollado con conectores
    using namespace Ilus;
    float e = L.escala, radio = 34 * e;
    Rectangle a = L.area;
    Vector2 rollo{L.centro.x, L.centro.y + 3 * e};
    DrawCircleLinesV(rollo, radio, tinta);
    DrawCircleLinesV(rollo, radio - 7 * e, brillo);
    DrawCircleLinesV(rollo, radio - 14 * e, tinta);
    DrawLineEx(Vector2{rollo.x - radio, rollo.y}, Vector2{a.x + a.width * 0.20f, rollo.y}, 6 * e, tinta);
    DrawRectangleRounded(R(a.x + a.width * 0.11f, rollo.y - 10 * e, 34 * e, 20 * e), 0.18f, 5, tinta);
    DrawRectangleRounded(R(a.x + a.width * 0.72f, rollo.y - 8 * e, 38 * e, 16 * e), 0.18f, 5, brillo);
    DrawRectangle((int)(a.x + a.width * 0.82f), (int)(rollo.y - 5 * e), (int)(12 * e), (int)(10 * e), Al(WHITE, 0.9f));
    DrawLineEx(Vector2{rollo.x + radio, rollo.y}, Vector2{a.x + a.width * 0.76f, rollo.y}, 6 * e, tinta);
}

static void dibujarCargador(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-22, -64, 9, 22, 0.2f, metal);  // clavijas
    L.rect(13, -64, 9, 22, 0.2f, metal);
    L.rect(-40, -44, 80, 90, 0.1f, tinta);
    L.rect(-36, -40, 72, 82, 0.08f, claro);
    L.texto("USB-C", -36, -34, 72, 24, 14, tinta);
    L.rect(-20, 0, 40, 6, 0.5f, L.acento);
    L.rect(-14, 24, 28, 9, 0.5f, tinta);
}

static void dibujarWindows(const Lienzo& L) {
    using namespace Ilus;
    L.cajaSoftware(Color{0, 120, 215, 255});
    L.rect(-19, -42, 24, 24, 0.08f, blanco);
    L.rect(8, -42, 24, 24, 0.08f, blanco);
    L.rect(-19, -15, 24, 24, 0.08f, blanco);
    L.rect(8, -15, 24, 24, 0.08f, blanco);
    L.texto("Windows", -34, 22, 82, 22, 15, blanco);
}

static void dibujarAdobe(const Lienzo& L) {
    L.cajaSoftware(Color{200, 30, 45, 255});
    L.texto("A", -34, -52, 82, 74, 64, Ilus::blanco);
    L.texto("Adobe", -34, 28, 82, 22, 15, Ilus::blanco);
}

static void dibujarAvast(const Lienzo& L) {
    Color naranja{255, 110, 0, 255};
    L.cajaSoftware(naranja);
    L.circulo(7, -20, 28, Ilus::blanco);
    L.circulo(7, -20, 20, naranja);
    L.texto("a", -20, -46, 54, 50, 40, Ilus::blanco);
    L.texto("avast", -34, 28, 82, 22, 15, Ilus::blanco);
}

static void dibujarOffice(const Lienzo& L) {
    Color rojo{215, 60, 10, 255};
    L.cajaSoftware(rojo);
    L.rect(-18, -50, 50, 62, 0.1f, Ilus::blanco);
    L.texto("O", -18, -50, 50, 62, 42, rojo);
    L.texto("Office", -34, 28, 82, 22, 15, Ilus::blanco);
}

static void dibujarSoftwareGenerico(const Lienzo& L) {
    using namespace Ilus;
    L.cajaSoftware(brillo);
    L.circulo(7, -14, 28, claro);
    L.circulo(7, -14, 9, L.acento);
    L.aro(7, -14, 20, brillo);
    L.texto("Software", -34, 28, 82, 22, 14, blanco);
}

static void dibujarLaptop(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-66, -56, 132, 88, 0.06f, tinta);
    L.rect(-59, -49, 118, 74, 0.04f, claro);
    L.rect(-40, -34, 80, 44, 0.06f, brillo);
    L.circulo(0, -12, 8, L.acento);
    L.rect(-88, 32, 176, 14, 0.5f, brillo);
    L.rect(-24, 32, 48, 6, 0.5f, tinta);
}

static void dibujarMonitor(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-82, -58, 164, 104, 0.05f, tinta);
    L.rect(-75, -51, 150, 90, 0.03f, claro);
    L.rect(-60, -38, 70, 8, 0.5f, brillo);
    L.rect(-60, -24, 110, 6, 0.5f, celeste);
    L.rect(-60, -10, 50, 34, 0.06f, brillo);
    L.circulo(-35, 7, 8, L.acento);
    L.rect(-10, 46, 20, 16, 0.1f, tinta);  // pie
    L.rect(-36, 60, 72, 8, 0.4f, tinta);
}

static void dibujarImpresora(const Lienzo& L) {
    using namespace Ilus;
    L.rect(-41, -62, 82, 48, 0.04f, celeste);  // hoja de arriba
    L.rect(-39, -60, 78, 46, 0.04f, blanco);
    L.linea(-28, -48, 28, -48, 2, claro);
    L.linea(-28, -38, 28, -38, 2, claro);
    L.linea(-28, -28, 10, -28, 2, claro);
    L.rect(-76, -20, 152, 66, 0.12f, tinta);  // cuerpo
    L.rect(-76, -20, 152, 14, 0.4f, brillo);
    L.rect(-52, 26, 104, 6, 0.4f, oscuro);
    L.rect(-62, 0, 44, 8, 0.4f, brillo);
    L.circulo(60, 4, 4, C_OK);
    L.rect(-42, 28, 84, 34, 0.03f, blanco);  // hoja de salida
    L.linea(-30, 40, 30, 40, 2, claro);
    L.linea(-30, 50, 14, 50, 2, claro);
}

static void dibujarComputadora(const Lienzo& L) {  // torre + monitor
    using namespace Ilus;
    L.rect(-92, -60, 52, 120, 0.1f, tinta);
    L.rect(-86, -30, 40, 60, 0.08f, oscuro);
    L.linea(-80, -20, -52, -20, 2, L.acento);
    L.linea(-80, -8, -58, -8, 2, brillo);
    L.linea(-80, 4, -54, 4, 2, brillo);
    L.circulo(-66, -46, 5, claro);
    L.circulo(-66, -46, 2, C_OK);
    for (int i = 0; i < 3; i++)
        L.linea(-82, 40 + i * 6.0f, -50, 40 + i * 6.0f, 2, brillo);
    L.rect(-28, -52, 118, 76, 0.06f, tinta);
    L.rect(-22, -46, 106, 64, 0.04f, claro);
    L.rect(-14, -38, 40, 6, 0.5f, brillo);
    L.rect(-14, -26, 70, 5, 0.5f, celeste);
    L.circulo(50, 0, 8, L.acento);
    L.rect(24, 24, 16, 22, 0.1f, tinta);
    L.rect(8, 44, 48, 8, 0.4f, tinta);
}

static void dibujarMantenimiento(const Lienzo& L) {  // engranaje y llave
    using namespace Ilus;
    float e = L.escala;
    for (int i = 0; i < 8; i++)  // dientes del engranaje
        DrawRectanglePro(R(L.centro.x - 10 * e + cosf(i * 45 * DEG2RAD) * 34 * e, L.centro.y + sinf(i * 45 * DEG2RAD) * 34 * e,
                           16 * e, 14 * e),
                         Vector2{8 * e, 7 * e}, (float)(i * 45), tinta);
    L.circulo(-10, 0, 30, tinta);
    L.circulo(-10, 0, 12, claro);
    L.circulo(-10, 0, 5, L.acento);
    L.linea(28, 44, 68, -4, 10, brillo);  // llave
    L.circulo(72, -12, 13, brillo);
    L.circulo(77, -19, 6, claro);
}

static void dibujarGarantia(const Lienzo& L) {  // escudo con visto
    using namespace Ilus;
    float e = L.escala;
    L.rect(-44, -52, 88, 64, 0.08f, tinta);
    DrawTriangle(Vector2{L.centro.x - 44 * e, L.centro.y + 8 * e}, Vector2{L.centro.x, L.centro.y + 56 * e},
                 Vector2{L.centro.x + 44 * e, L.centro.y + 8 * e}, tinta);
    L.linea(-18, 0, -4, 14, 7, blanco);
    L.linea(-4, 14, 22, -18, 7, blanco);
    L.circulo(0, -34, 4, L.acento);
}

static void dibujarGenerico(const Lienzo& L) {
    L.rect(-48, -48, 96, 96, 0.1f, Ilus::claro);
    L.rect(-34, -34, 68, 68, 0.08f, Ilus::tinta);
    L.circulo(0, 0, 10, L.acento);
}

// Punto de entrada: foto del producto si existe; si no, la ilustración que corresponda a su tipo.
static void arte(const Producto* producto, Rectangle area, Color acento) {
    if (imagen("img/productos/" + producto->codigo() + ".png", area))
        return;

    Lienzo L(area, acento);

    // Halo y sombra de fondo
    CirculoDeg(L.centro, min(area.width, area.height) * 0.48f, Al(acento, 0.10f), Al(acento, 0.015f));
    DrawEllipse((int)L.centro.x, (int)(L.centro.y + 68 * L.escala), (int)(72 * L.escala), (int)(6 * L.escala),
                Al(Ilus::tinta, 0.10f));

    // Decide el tipo: primero por nombre; si no, por nombre + detalle; si no, por categoría
    string descripcion = minusc(producto->nombre() + " " + producto->detalle());
    string tipo = clasificar(minusc(producto->nombre()));
    if (tipo.empty())
        tipo = clasificar(descripcion);
    if (tipo.empty()) {
        const char* porCategoria[6] = {"cpu", "cargador", "mouse", "software", "laptop", "mant"};
        tipo = porCategoria[(int)Clamp((float)producto->categoria(), 0, 5)];
    }

    bool esM2 = tipo == "m2" || (tipo == "ssd" && (descripcion.find("m.2") != string::npos || descripcion.find("nvme") != string::npos));

    if (tipo == "gpu")
        dibujarGPU(L);
    else if (tipo == "cpu")
        dibujarCPU(L);
    else if (tipo == "ram")
        dibujarRAM(L);
    else if (esM2)
        dibujarM2(L);
    else if (tipo == "ssd")
        dibujarSSD(L);
    else if (tipo == "hdd")
        dibujarHDD(L);
    else if (tipo == "placa" || tipo == "armado")
        dibujarPlaca(L, tipo == "armado");
    else if (tipo == "psu")
        dibujarFuente(L);
    else if (tipo == "fan")
        dibujarVentilador(L);
    else if (tipo == "usb")
        dibujarUSB(L);
    else if (tipo == "sd")
        dibujarSD(L);
    else if (tipo == "pasta")
        dibujarPasta(L);
    else if (tipo == "mouse")
        dibujarMouse(L, descripcion);
    else if (tipo == "teclado")
        dibujarTeclado(L);
    else if (tipo == "parlante")
        dibujarParlante(L);
    else if (tipo == "camara")
        dibujarCamara(L);
    else if (tipo == "mic")
        dibujarMicrofono(L);
    else if (tipo == "wifi")
        dibujarWifi(L);
    else if (tipo == "adaptador")
        dibujarAdaptador(L);
    else if (tipo == "cable")
        dibujarCable(L);
    else if (tipo == "cargador")
        dibujarCargador(L);
    else if (tipo == "windows")
        dibujarWindows(L);
    else if (tipo == "adobe")
        dibujarAdobe(L);
    else if (tipo == "avast")
        dibujarAvast(L);
    else if (tipo == "office")
        dibujarOffice(L);
    else if (tipo == "software")
        dibujarSoftwareGenerico(L);
    else if (tipo == "laptop")
        dibujarLaptop(L);
    else if (tipo == "monitor")
        dibujarMonitor(L);
    else if (tipo == "impresora")
        dibujarImpresora(L);
    else if (tipo == "pc")
        dibujarComputadora(L);
    else if (tipo == "mant")
        dibujarMantenimiento(L);
    else if (tipo == "garantia")
        dibujarGarantia(L);
    else
        dibujarGenerico(L);
}

// =====================================================================================
// 8. INTERFAZ: elementos y pantallas
// =====================================================================================

// ---- Elementos de interfaz (abstracción + herencia + polimorfismo) ----
class ElementoUI {
protected:
    Rectangle rect_{};
    float hover_ = 0;  // 0..1, animación suave al pasar el mouse

public:
    virtual ~ElementoUI() = default;

    void mover(Rectangle r) {
        rect_ = r;
    }

    bool sobre(Vector2 mouse) const {
        return CheckCollisionPointRec(mouse, rect_);
    }

    void actualizar(float dt, Vector2 mouse) {
        hover_ = Lerp(hover_, sobre(mouse) ? 1.0f : 0.0f, Clamp(dt * 12, 0, 1));
    }

    virtual void dibujar(bool conFoco) const = 0;
};

// Cuadro de texto. Modos: 0 = cualquier texto, 1 = solo dígitos, 2 = decimal (dígitos y punto)
class Campo : public ElementoUI {
    size_t maxCaracteres_;
    int modo_;

public:
    string etiqueta, pista, texto, error;

    Campo(string etiqueta, size_t maxCaracteres, int modo = 0)
        : maxCaracteres_(maxCaracteres), modo_(modo), etiqueta(move(etiqueta)) {
    }

    void teclear(int caracter) {
        bool permitido = modo_ == 1   ? (caracter < 128 && isdigit(caracter))
                         : modo_ == 2 ? (caracter < 128 && (isdigit(caracter) || caracter == '.'))
                                      : (caracter >= 32 && caracter != 127 && caracter < 256);
        if (!permitido || texto.size() >= maxCaracteres_)
            return;
        int bytes = 0;
        texto += CodepointToUTF8(caracter, &bytes);  // admite ñ y tildes (UTF-8)
    }

    void borrar() {
        while (!texto.empty() && ((unsigned char)texto.back() & 0xC0) == 0x80)  // bytes de continuación UTF-8
            texto.pop_back();
        if (!texto.empty())
            texto.pop_back();
    }

    void dibujar(bool conFoco) const override {
        Color borde = !error.empty() ? C_PELIGRO : conFoco ? C_PRIM : Mezcla(C_LINEA, C_MUTED, hover_ * 0.5f);
        if (!etiqueta.empty())
            Texto(etiqueta, rect_.x + 2, rect_.y - 20, 12, C_MUTED);
        Caja(rect_, 0.2f, Al(WHITE, 0.85f), borde);
        if (conFoco)
            LineasR(R(rect_.x - 1, rect_.y - 1, rect_.width + 2, rect_.height + 2), 0.2f, 12, 1, Al(borde, 0.35f));

        string visible = Ajustar(texto, 15, rect_.width - 28);
        float yTexto = rect_.y + (rect_.height - 15) / 2 - 1;
        if (texto.empty())
            Texto(pista, rect_.x + 14, yTexto, 15, Al(C_MUTED, 0.7f));
        else
            Texto(visible, rect_.x + 14, yTexto, 15, C_TINTA);

        bool cursorVisible = conFoco && fmod(GetTime(), 1.0) < 0.5;  // cursor parpadeante
        if (cursorVisible)
            DrawRectangle((int)(rect_.x + 15 + (texto.empty() ? 0 : Ancho(visible, 15))), (int)(rect_.y + 10), 2,
                          (int)rect_.height - 20, C_PRIM);
        if (!error.empty())
            Texto(error, rect_.x + 2, rect_.y + rect_.height + 4, 11, C_PELIGRO);
    }
};

// ---- Interfaz: ventana principal, pantallas y capas (carrito, checkout, detalle) ----
class Interfaz {
    // --- Dimensiones lógicas (la interfaz se escala para llenar la ventana) ---
    static constexpr float TOP = 64;  // alto de la barra superior
    float W = 1280, H = 800, SC = 1;  // W,H = tamaño lógico; SC = escala

    const Vector2 OFF = {-9999, -9999};  // "mouse fuera de pantalla" para capas tapadas
    Tienda t_;

    // --- Textos y colores fijos ---
    const char* tabs_[7] = {"Inicio", "Productos", "Ofertas", "Servicios", "Pedidos", "Nosotros", "Inventario"};
    const char* cats_[6] = {"Hardware", "Accesorios", "Periféricos", "Software", "Equipos", "Servicios"};
    Color ac_[6] = {{99, 102, 241, 255}, {236, 72, 153, 255}, {16, 185, 129, 255},
                    {245, 158, 11, 255}, {14, 165, 233, 255}, {244, 63, 94, 255}};  // color de cada categoría

    // --- Datos de pantalla ---
    vector<const Producto*> destacados_, recomendados_;
    vector<string> marcas_, modelosElegidos_;
    unordered_map<int, float> hov_;          // animaciones hover por id de botón
    vector<function<void()>> cola_;          // acciones diferidas (se ejecutan al final del cuadro)

    // --- Campos de texto ---
    Campo busca_{"", 24}, cNom_{"Nombres", 30}, cApP_{"Apellido paterno", 20}, cApM_{"Apellido materno", 20}, cDni_{"DNI", 8, 1},
        cTel_{"Celular", 9, 1}, cMail_{"Correo electrónico", 40}, cDir_{"Dirección de entrega", 50}, cRuc_{"RUC", 11, 1},
        cRazon_{"Razón social", 40}, cPago_{"", 12, 2}, cProdNombre_{"Nombre del producto", 40}, cProdMarca_{"Marca", 30},
        cProdPrecio_{"Precio (S/)", 10, 2}, cProdStock_{"Unidades iniciales", 6, 1}, cProdDetalle_{"Descripción breve", 60},
        cModelo_{"", 24}, cCupon_{"Cupón de descuento", 20}, cRepo_{"Unidades a reponer", 5, 1};

    // --- Filtros y estado de texto ---
    string prod_;      // producto elegido en la barra lateral (ej. "CPU"); vacío = todos
    string marca_;     // marca elegida; vacío = todas
    string prevBusca_, prevModelo_, toastMsg_;

    // --- Desplazamientos y animaciones ---
    float sideScroll_ = 0, sideMax_ = 0;       // scroll de la barra lateral
    float capaA_[4] = {};                      // opacidad animada de cada capa (1 = carrito, 2 = checkout, 3 = detalle)
    float scroll_ = 0, scrollObj_ = 0, contentH_ = 0;  // scroll de la página
    float heroT_ = 0, heroFade_ = 1, bump_ = 0, toast_ = 0, toastA_ = 0, dt_ = 0.016f, wheel_ = 0;

    // --- Banderas del cuadro actual ---
    bool sobreBarra_ = false, click_ = false, enter_ = false, manito_ = false, ibeam_ = false, clicCampo_ = false;

    // --- Estado de navegación ---
    int paginaMarcas_ = 0, modeloOff_ = 0;
    int tab_ = 0;                 // pestaña actual (0 = Inicio ... 6 = Inventario)
    int cat_ = -1;                // categoría elegida (-1 = todas)
    int orden_ = 0;               // 0 original, 1 menor precio, 2 mayor precio
    int capa_ = 0, capaSig_ = 0;  // capa activa: 0 página, 1 carrito, 2 checkout, 3 detalle de producto
    int paso_ = 0;                // paso del checkout (0 datos, 1 entrega, 2 pago, 3 listo)
    int vend_ = 0;                // vendedor elegido
    int heroIdx_ = 0, modeloSel_ = 0, cantSel_ = 1, scrollCar_ = 0, catNueva_ = 0;

    Campo* foco_ = nullptr;       // campo que tiene el teclado
    Pedido ultimo_;               // último pedido pagado (se muestra en el paso "Listo")
    const Producto* sel_ = nullptr;
    Vector2 M = {0, 0};           // mouse de la capa que se está dibujando (OFF si está tapada)
    Vector2 mouse_ = {0, 0};

    // =============== utilidades ===============

    // Animación suave 0..1 asociada a un id (para hover)
    float anim(int id, bool activo) {
        float& v = hov_[id];
        v = Lerp(v, activo ? 1.0f : 0.0f, Clamp(dt_ * 12, 0, 1));
        return v;
    }

    bool sobre(Rectangle r) const {
        return CheckCollisionPointRec(M, r);
    }

    // Programa una acción para el final del cuadro (evita modificar datos mientras se dibuja)
    void luego(function<void()> accion) {
        cola_.push_back(move(accion));
    }

    void ir(int capa) {
        capaSig_ = capa;
    }

    void aviso(const string& mensaje, float segundos = 2.2f) {
        toastMsg_ = mensaje;
        toast_ = segundos;
    }

    void abrir(const Producto* producto) {  // abre la ventana de detalle
        sel_ = producto;
        modeloSel_ = producto->recomendado();
        cantSel_ = 1;
        cModelo_.texto.clear();
        prevModelo_.clear();
        modeloOff_ = max(0, modeloSel_ - 2);
        ir(3);
    }

    void cambiarHero(int i) {
        heroIdx_ = i;
        heroT_ = 0;
        heroFade_ = 0;
    }

    void filtro(int tab, int cat, const string& prod, const string& marca) {
        tab_ = tab;
        cat_ = cat;
        prod_ = prod;
        marca_ = marca;
        scrollObj_ = 0;
    }

    int columnas(float anchoContenido) const {
        return max(2, (int)((anchoContenido + 16) / 236));
    }

    void velo(float alfa, float intensidad) const {  // capa oscura detrás de las ventanas emergentes
        DrawRectangle(0, 0, (int)W + 2, (int)H + 2, Al(Color{20, 26, 70, 255}, intensidad * alfa));
    }

    // =============== componentes reutilizables ===============

    // Botón redondeado. Devuelve true si se hizo clic en él.
    bool boton(int id, Rectangle r, const string& texto, Color color, bool activo = true, float tam = 15) {
        bool encima = activo && sobre(r);
        float k = anim(id, encima);
        bool colorClaro = color.r * 0.299f + color.g * 0.587f + color.b * 0.114f > 160;
        Rectangle q = R(r.x, r.y - k * 1.5f, r.width, r.height);
        if (activo && !colorClaro)
            DrawRectangleRounded(R(r.x + 3, r.y + 5, r.width - 6, r.height - 4), 0.5f, 12, Al(color, 0.28f));
        DrawRectangleRounded(q, 0.5f, 12, activo ? Mezcla(color, WHITE, k * 0.15f) : Color{225, 230, 243, 255});
        if (colorClaro)
            LineasR(q, 0.5f, 12, 1, C_LINEA);
        TextoCentro(texto, q, tam, !activo ? C_MUTED : colorClaro ? C_TINTA : WHITE);
        if (encima)
            manito_ = true;
        return encima && click_;
    }

    // Chip seleccionable (la opción elegida se rellena de azul marino)
    bool chip(int id, Rectangle r, const string& texto, bool seleccionado, Color colorBorde = C_PRIM) {
        bool encima = sobre(r);
        float k = anim(id, encima);
        Caja(r, 0.4f, seleccionado ? C_PRIM : Mezcla(Al(WHITE, 0.7f), C_SUAVE, k),
             seleccionado ? colorBorde : Mezcla(C_LINEA, C_MUTED, k * 0.5f));
        TextoCentro(texto, r, 13, seleccionado ? WHITE : Mezcla(C_MUTED, C_TINTA, k));
        if (encima)
            manito_ = true;
        return encima && click_;
    }

    // Botón de cerrar (X)
    bool equis(Rectangle r) {
        bool encima = sobre(r);
        Vector2 c = {r.x + r.width / 2, r.y + r.height / 2};
        DrawCircleV(c, r.width / 2, encima ? C_SUAVE : Al(WHITE, 0.8f));
        DrawLineEx(Vector2{c.x - 5, c.y - 5}, Vector2{c.x + 5, c.y + 5}, 2, C_TINTA);
        DrawLineEx(Vector2{c.x + 5, c.y - 5}, Vector2{c.x - 5, c.y + 5}, 2, C_TINTA);
        if (encima)
            manito_ = true;
        return encima && click_;
    }

    // Coloca un campo de texto, gestiona el clic (foco) y lo dibuja
    void campo(Campo& c, Rectangle r) {
        c.mover(r);
        c.actualizar(dt_, M);
        if (c.sobre(M)) {
            ibeam_ = true;
            if (click_) {
                foco_ = &c;
                clicCampo_ = true;
            }
        }
        c.dibujar(foco_ == &c);
    }

    void tag(Rectangle r, const string& texto, Color color) {  // etiqueta pequeña de color
        DrawRectangleRounded(r, 0.5f, 8, Al(color, 0.18f));
        TextoCentro(texto, r, 10, color);
    }

    // Casilla con visto (invertida = sobre fondo azul)
    void casilla(Rectangle cb, bool marcada, bool invertida) {
        DrawRectangleRounded(cb, 0.25f, 6, marcada ? (invertida ? WHITE : C_PRIM) : Al(WHITE, 0.9f));
        LineasR(cb, 0.25f, 6, 1, marcada ? (invertida ? WHITE : C_PRIM) : C_LINEA);
        if (!marcada)
            return;
        float w = cb.width;
        Color visto = invertida ? C_PRIM : WHITE;
        DrawLineEx(Vector2{cb.x + w * 0.2f, cb.y + w * 0.5f}, Vector2{cb.x + w * 0.45f, cb.y + w * 0.75f}, 2, visto);
        DrawLineEx(Vector2{cb.x + w * 0.45f, cb.y + w * 0.75f}, Vector2{cb.x + w * 0.8f, cb.y + w * 0.25f}, 2, visto);
    }

    Color estadoColor(const Producto* p) const {
        return !p->disponible() ? C_PELIGRO : p->stock() <= 5 ? C_AVISO : C_OK;
    }

    // Productos que cumplen pestaña, categoría, producto, marca y búsqueda actuales (ordenados si corresponde)
    vector<const Producto*> filtrar() const {
        vector<const Producto*> resultado;
        string busqueda = minusc(busca_.texto);
        for (const Producto* p : t_.productos()) {
            bool noEsOferta = tab_ == 2 && !p->enOferta();
            bool noEsServicio = tab_ == 3 && p->categoria() != 5;
            bool otraCategoria = tab_ == 1 && cat_ >= 0 && p->categoria() != cat_;
            if (noEsOferta || noEsServicio || otraCategoria)
                continue;
            bool otroProducto = tab_ == 1 && !prod_.empty() && p->nombre() != prod_;
            bool otraMarca = !marca_.empty() && p->marca() != marca_;
            if (otroProducto || otraMarca)
                continue;
            if (!busqueda.empty() && minusc(p->nombre() + " " + p->marca() + " " + p->detalle()).find(busqueda) == string::npos)
                continue;
            resultado.push_back(p);
        }
        if (orden_)
            stable_sort(resultado.begin(), resultado.end(), [this](const Producto* a, const Producto* b) {
                return orden_ == 1 ? desdePrecio(a) < desdePrecio(b) : desdePrecio(a) > desdePrecio(b);
            });
        return resultado;
    }

    // =============== tarjeta de producto ===============
    void tarjeta(const Producto* p, Rectangle r, int id) {
        bool encima = sobre(r);
        float k = anim(id, encima);
        Rectangle q = R(r.x, r.y - k * 4, r.width, r.height);  // se eleva al pasar el mouse
        Color ac = ac_[p->categoria()];
        Vidrio(q, 0.06f, 0.5f + k);
        if (k > 0.05f)
            LineasR(q, 0.06f, 12, 2, Al(C_PRIM, k * 0.9f));

        // imagen
        Rectangle im = R(q.x + 8, q.y + 8, q.width - 16, 128);
        DrawRectangleRounded(im, 0.1f, 10, Al(Mezcla(WHITE, ac, 0.14f), 0.85f));
        arte(p, im, ac);
        if (p->enOferta()) {
            DrawRectangleRounded(R(q.x + 16, q.y + 16, 62, 22), 0.5f, 8, C_OK);
            TextoCentro("OFERTA", R(q.x + 16, q.y + 16, 62, 22), 11, WHITE);
        }
        Marca(p->marca(), R(q.x + q.width - 98, q.y + 16, 82, 22));

        // textos
        Texto(cats_[p->categoria()], q.x + 14, q.y + 143, 11, ac);
        Texto(Ajustar(p->nombre(), 17, q.width - 28), q.x + 14, q.y + 158, 17, C_TINTA);
        Texto(Ajustar(p->detalle(), 12, q.width - 28), q.x + 14, q.y + 183, 12, C_MUTED);

        // pie: precio y estado
        DrawLine((int)q.x + 14, (int)(q.y + q.height - 54), (int)(q.x + q.width - 14), (int)(q.y + q.height - 54), Al(C_LINEA, 0.9f));
        Texto("Desde", q.x + 14, q.y + q.height - 46, 11, C_MUTED);
        Texto(dinero(desdePrecio(p)), q.x + 14, q.y + q.height - 32, 18, C_PRIM);
        string estado = p->estado();
        TextoDer(estado, q.x + q.width - 14, q.y + q.height - 30, 12, C_MUTED);
        DrawCircleV(Vector2{q.x + q.width - 14 - Ancho(estado, 12) - 10, q.y + q.height - 23}, 4, estadoColor(p));

        if (encima) {
            manito_ = true;
            if (click_)
                luego([this, p] {
                    abrir(p);
                });
        }
    }

    // Encabezado de sección con barra de color
    void titulo(float x, float y, const string& texto, const string& subtitulo) {
        DrawRectangleRounded(R(x, y + 3, 4, 24), 1.0f, 4, C_ACENTO);
        Texto(texto, x + 14, y, 22, C_PRIM);
        Texto(subtitulo, x + 14 + Ancho(texto, 22) + 14, y + 7, 13, C_MUTED);
    }

    // Franja azul suave que encabeza cada sección de la barra lateral
    void seccion(float x, float w, float& y, const string& texto) {
        DrawRectangleRounded(R(x + 8, y, w - 16, 30), 0.3f, 8, Al(C_PRIM, 0.10f));
        DrawRectangle((int)x + 8, (int)y + 6, 3, 18, C_PRIM);
        Texto(texto, x + 20, y + 7, 14, C_PRIM);
        y += 38;
    }

    // =============== barra lateral (Inicio y Productos): producto > marcas ===============
    void barraLateral(float x, float w) {
        float y = TOP + 16, h = H - TOP - 32;
        Rectangle panel = R(x, y, w, h), area = R(x, y + 52, w, h - 56);
        Vidrio(panel, 0.05f, 0.6f);
        DrawRectangleRounded(R(x, y, w, 46), 0.3f, 10, C_PRIM);
        for (int k = 0; k < 3; k++)  // icono de menú
            DrawRectangle((int)x + 16, (int)y + 15 + k * 6, 14, 2, WHITE);
        Texto("Catálogo de productos", x + 42, y + 13, 15, WHITE);

        if (sobre(panel)) {
            sobreBarra_ = true;
            if (wheel_ != 0)
                sideScroll_ = Clamp(sideScroll_ - wheel_ * 40, 0, sideMax_);
        }
        sideScroll_ = Clamp(sideScroll_, 0, sideMax_);

        // Un elemento solo recibe clic si el mouse está dentro del área visible de la lista
        auto clicable = [&](Rectangle r) {
            return sobre(r) && sobre(area);
        };

        auto prods = t_.productos();
        float inicioLista = area.y + 6 - sideScroll_, yy = inicioLista;
        BeginScissorMode((int)(area.x * SC), (int)(area.y * SC), (int)(area.width * SC), (int)(area.height * SC));

        // ---- Sección "Producto": categorías (y sus productos si está abierta) ----
        seccion(x, w, yy, "Producto");
        for (int i = -1; i < 6; i++) {  // -1 = "Todos los productos"
            int cantidad = 0;
            for (auto* p : prods)
                if (i < 0 || p->categoria() == i)
                    cantidad++;

            Rectangle fila = R(x + 10, yy, w - 20, 32);
            bool seleccionada = tab_ != 0 && cat_ == i && prod_.empty();
            bool abiertaConProducto = cat_ == i && !prod_.empty();
            bool encima = clicable(fila);
            float k = anim(1100 + i + 1, encima);
            if (seleccionada)
                DrawRectangleRounded(fila, 0.3f, 8, C_PRIM);
            else if (abiertaConProducto)
                DrawRectangleRounded(fila, 0.3f, 8, Al(C_PRIM, 0.12f));
            else if (k > 0.02f)
                DrawRectangleRounded(fila, 0.3f, 8, Al(C_SUAVE, k));
            DrawCircleV(Vector2{fila.x + 16, fila.y + 16}, 5, i < 0 ? C_ACENTO : ac_[i]);
            Texto(i < 0 ? "Todos los productos" : cats_[i], fila.x + 32, fila.y + 8, 14, seleccionada ? WHITE : C_PRIM);
            TextoDer(to_string(cantidad), fila.x + fila.width - 12, fila.y + 9, 12, seleccionada ? Al(WHITE, 0.85f) : C_MUTED);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        filtro(1, i, "", "");
                    });
            }
            yy += 34;

            // Productos de la categoría abierta (CPU, GPU, RAM...)
            if (cat_ == i && i >= 0) {
                vector<pair<string, int>> nombres;  // (nombre, cuántos hay)
                for (auto* p : prods)
                    if (p->categoria() == i) {
                        auto it = find_if(nombres.begin(), nombres.end(), [&](const pair<string, int>& a) {
                            return a.first == p->nombre();
                        });
                        if (it == nombres.end())
                            nombres.push_back({p->nombre(), 1});
                        else
                            it->second++;
                    }
                for (int j = 0; j < (int)nombres.size(); j++) {
                    const string nombre = nombres[(size_t)j].first;
                    Rectangle filaProd = R(x + 22, yy, w - 34, 28);
                    bool elegido = prod_ == nombre, encimaProd = clicable(filaProd);
                    float kp = anim(1300 + j, encimaProd);
                    if (elegido)
                        DrawRectangleRounded(filaProd, 0.3f, 8, C_PRIM);
                    else if (kp > 0.02f)
                        DrawRectangleRounded(filaProd, 0.3f, 8, Al(C_SUAVE, kp));
                    casilla(R(filaProd.x + 8, filaProd.y + 7, 14, 14), elegido, true);
                    Texto(Ajustar(nombre, 13, w - 100), filaProd.x + 30, filaProd.y + 7, 13, elegido ? WHITE : C_TINTA);
                    TextoDer(to_string(nombres[(size_t)j].second), filaProd.x + filaProd.width - 10, filaProd.y + 8, 12,
                             elegido ? Al(WHITE, 0.85f) : C_MUTED);
                    if (encimaProd) {
                        manito_ = true;
                        if (click_)
                            luego([this, nombre] {
                                filtro(1, cat_, prod_ == nombre ? string() : nombre, "");
                            });
                    }
                    yy += 30;
                }
                yy += 4;
            }
        }

        // ---- Sección "Marca": marcas disponibles con logo ----
        yy += 8;
        seccion(x, w, yy, "Marca");
        vector<pair<string, int>> marcas;
        for (auto* p : prods)
            if ((cat_ < 0 || p->categoria() == cat_) && (prod_.empty() || p->nombre() == prod_)) {
                auto it = find_if(marcas.begin(), marcas.end(), [&](const pair<string, int>& a) {
                    return a.first == p->marca();
                });
                if (it == marcas.end())
                    marcas.push_back({p->marca(), 1});
                else
                    it->second++;
            }
        sort(marcas.begin(), marcas.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return minusc(a.first) < minusc(b.first);
        });
        for (int i = 0; i < (int)marcas.size(); i++) {
            const string nombre = marcas[(size_t)i].first;
            Rectangle fila = R(x + 10, yy, w - 20, 36);
            bool elegida = marca_ == nombre, encima = clicable(fila);
            float k = anim(1500 + i, encima);
            if (elegida)
                DrawRectangleRounded(fila, 0.3f, 8, Al(C_PRIM, 0.12f));
            else if (k > 0.02f)
                DrawRectangleRounded(fila, 0.3f, 8, Al(C_SUAVE, k));
            casilla(R(fila.x + 8, fila.y + 10, 16, 16), elegida, false);
            Marca(nombre, R(fila.x + 34, fila.y + 5, 96, 26));
            TextoDer(to_string(marcas[(size_t)i].second), fila.x + fila.width - 12, fila.y + 11, 12, C_MUTED);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, nombre] {
                        tab_ = 1;
                        marca_ = marca_ == nombre ? string() : nombre;
                        scrollObj_ = 0;
                    });
            }
            yy += 38;
        }
        if (marcas.empty())
            Texto("Sin marcas", x + 20, yy, 12, C_MUTED);
        EndScissorMode();

        // Barra de desplazamiento de la lista
        float contenido = yy - inicioLista + 14;
        sideMax_ = max(0.0f, contenido - area.height);
        if (sideMax_ > 0) {
            float altoBarra = max(30.0f, area.height * area.height / contenido);
            DrawRectangleRounded(R(x + w - 7, area.y + 2 + sideScroll_ / sideMax_ * (area.height - 4 - altoBarra), 3, altoBarra), 1.0f,
                                 4, Al(C_PRIM, 0.45f));
        }
    }

    // =============== páginas ===============
    // Cada página recibe (ox = x inicial, cw = ancho de contenido, y = y inicial) y devuelve la y final.

    float pie(float ox, float cw, float y) {
        DrawLine((int)ox, (int)y, (int)(ox + cw), (int)y, Al(C_LINEA, 0.9f));
        Texto("TecnoStore · Tienda de computadoras", ox, y + 18, 13, C_TINTA);
        TextoDer("Lun a Sáb 9:00 - 20:00  ·  951 000 000  ·  ventas@tecnostore.pe", ox + cw, y + 19, 12, C_MUTED);
        return y + 56;
    }

    // ---------- Página: Inicio ----------
    float inicio(float ox, float cw, float y) {
        barraLateral(ox, 240);
        ox += 264;
        cw -= 264;

        // --- Banner principal (rota entre productos destacados) ---
        const Producto* destacado = destacados_[(size_t)heroIdx_];
        Color ac = ac_[destacado->categoria()];
        float alfa = heroFade_, tiempo = (float)GetTime();
        Rectangle hero = R(ox, y, cw, 230);
        Sombra(hero, 0.1f, 1.5f);
        Deg(hero, 18, Mezcla(ac, Color{255, 70, 150, 255}, 0.5f), Mezcla(ac, Color{255, 150, 40, 255}, 0.55f), true);
        for (int k = 0; k < 5; k++)  // anillos decorativos animados
            DrawRing(Vector2{hero.x + hero.width * (0.5f + 0.09f * k), hero.y + 50 + k * 22 + sinf(tiempo * 0.9f + k) * 10},
                     (12.0f + k * 4) * 0.55f, 12.0f + k * 4, 0, 360, 32, Al(WHITE, 0.12f));
        DrawRectangleRounded(R(hero.x + 36, hero.y + 26, 120, 24), 0.5f, 10, Al(WHITE, 0.25f * alfa));
        TextoCentro("RECOMENDADO", R(hero.x + 36, hero.y + 26, 120, 24), 11, Al(WHITE, alfa));
        Texto(destacado->nombre(), hero.x + 36, hero.y + 62, 42, Al(WHITE, alfa));
        Texto(to_string(destacado->modelos().size()) + " modelos · desde " + dinero(desdePrecio(destacado)), hero.x + 37,
              hero.y + 118, 16, Al(WHITE, 0.9f * alfa));
        Rectangle botonModelos = R(hero.x + 36, hero.y + 164, 170, 44);
        Rectangle zonaArte = R(hero.x + hero.width - 420, hero.y + 8, 360, 214);
        if (boton(70, botonModelos, "Ver modelos", WHITE, true, 15))
            luego([this, destacado] {
                abrir(destacado);
            });
        arte(destacado, zonaArte, Mezcla(ac, WHITE, 0.5f));
        if (sobre(zonaArte) && !sobre(botonModelos)) {
            manito_ = true;
            if (click_)
                luego([this, destacado] {
                    abrir(destacado);
                });
        }
        Marca(destacado->marca(), R(hero.x + hero.width - 132, hero.y + 16, 104, 34), alfa);
        for (int i = 0; i < (int)destacados_.size(); i++) {  // puntos indicadores
            Rectangle punto = R(hero.x + 240 + i * 24.0f, hero.y + 172, 22, 20);
            if (i == heroIdx_)
                DrawRectangleRounded(R(punto.x, punto.y + 6, 20, 8), 0.5f, 8, WHITE);
            else
                DrawCircleV(Vector2{punto.x + 10, punto.y + 10}, 4, Al(WHITE, 0.5f));
            if (sobre(punto)) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        cambiarHero(i);
                    });
            }
        }
        y += 262;

        // --- Categorías: accesos directos ---
        titulo(ox, y, "Explora por categoría", "Elige qué tipo de producto buscas");
        y += 46;
        auto prods = t_.productos();
        int columnasCat = cw >= 1000 ? 6 : 3;
        float anchoCat = (cw - (columnasCat - 1) * 12) / columnasCat;
        for (int i = 0; i < 6; i++) {
            int cantidad = 0;
            for (auto* p : prods)
                if (p->categoria() == i)
                    cantidad++;
            Rectangle r = R(ox + (i % columnasCat) * (anchoCat + 12), y + (i / columnasCat) * 84.0f, anchoCat, 72);
            bool encima = sobre(r);
            float k = anim(1000 + i, encima);
            Rectangle q = R(r.x, r.y - k * 3, r.width, r.height);
            Vidrio(q, 0.14f, 0.4f + k * 0.6f);
            Rectangle icono = R(q.x + 12, q.y + 12, 48, 48);
            DrawRectangleRounded(icono, 0.3f, 8, Al(ac_[i], 0.16f + k * 0.14f));
            TextoCentro(string(cats_[i]).substr(0, 1), icono, 22, ac_[i]);
            Texto(cats_[i], q.x + 74, q.y + 16, 16, C_TINTA);
            Texto(to_string(cantidad) + (cantidad == 1 ? " producto" : " productos"), q.x + 74, q.y + 40, 12, C_MUTED);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        filtro(1, i, "", "");
                    });
            }
        }
        y += (6 + columnasCat - 1) / columnasCat * 84.0f + 18;

        // --- Recomendados ---
        titulo(ox, y, "Recomendados para ti", "Los más elegidos de la tienda");
        y += 46;
        int columnasProd = columnas(cw);
        float anchoTarjeta = (cw - (columnasProd - 1) * 16) / columnasProd;
        for (int i = 0; i < min(columnasProd, (int)recomendados_.size()); i++)
            tarjeta(recomendados_[(size_t)i], R(ox + i * (anchoTarjeta + 16), y, anchoTarjeta, 262), 4000 + i);
        y += 262 + 32;

        // --- Marcas: cuadrícula ordenada ---
        titulo(ox, y, "Marcas", "Toca una marca para ver sus productos");
        y += 46;
        int columnasMarca = max(3, (int)((cw + 10) / 170));
        float anchoMarca = (cw - (columnasMarca - 1) * 10) / columnasMarca;
        for (int i = 0; i < (int)marcas_.size(); i++) {
            const string& marca = marcas_[(size_t)i];
            int cantidad = 0;
            for (auto* p : prods)
                if (p->marca() == marca)
                    cantidad++;
            Rectangle r = R(ox + (i % columnasMarca) * (anchoMarca + 10), y + (i / columnasMarca) * 62.0f, anchoMarca, 52);
            bool encima = sobre(r);
            float k = anim(2000 + i, encima);
            Rectangle q = R(r.x, r.y - k * 3, r.width, r.height);
            Vidrio(q, 0.2f, 0.25f + k * 0.5f);
            Rectangle icono = R(q.x + 10, q.y + 9, 34, 34);
            if (!imagen("img/marcas/" + marca + ".png", icono)) {
                DrawCircleV(Vector2{icono.x + 17, icono.y + 17}, 17, colorMarca(marca));
                TextoCentro(marca.substr(0, 1), icono, 16, WHITE);
            }
            Texto(Ajustar(marca, 14, anchoMarca - 66), q.x + 54, q.y + 9, 14, C_TINTA);
            Texto(to_string(cantidad) + (cantidad == 1 ? " producto" : " productos"), q.x + 54, q.y + 30, 11, C_MUTED);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        filtro(1, -1, "", marcas_[(size_t)i]);
                    });
            }
        }
        y += ((int)marcas_.size() + columnasMarca - 1) / columnasMarca * 62.0f + 20;

        // --- Tres beneficios ---
        const char* titulos[3] = {"Pago a tu manera", "Entrega rápida", "Respaldo técnico"};
        const char* descripciones[3] = {"Efectivo, tarjeta, Yape y Plin", "Delivery a domicilio o recojo en tienda",
                                        "Garantía, mantenimiento y armado de PC"};
        float ancho3 = (cw - 32) / 3;
        for (int i = 0; i < 3; i++) {
            Rectangle r = R(ox + i * (ancho3 + 16), y, ancho3, 90);
            Vidrio(r, 0.12f, 0.5f);
            DrawCircleV(Vector2{r.x + 36, r.y + 45}, 18, Al(ac_[i * 2], 0.22f));
            DrawCircleV(Vector2{r.x + 36, r.y + 45}, 7, ac_[i * 2]);
            Texto(titulos[i], r.x + 70, r.y + 22, 17, C_TINTA);
            Texto(descripciones[i], r.x + 70, r.y + 50, 12, C_MUTED);
        }
        return pie(ox, cw, y + 90 + 36);
    }

    // ---------- Página: Productos / Ofertas / Servicios ----------
    float catalogo(float ox, float cw, float y) {
        if (tab_ == 1) {  // Productos: barra lateral fija a la izquierda
            barraLateral(ox, 240);
            ox += 264;
            cw -= 264;
        }
        auto lista = filtrar();
        int n = (int)lista.size();

        // Título según lo que esté filtrado
        const char* titulosPestana[4] = {"", "Productos", "Ofertas", "Servicios"};
        string titulo = tab_ == 1 && !prod_.empty() ? prod_ : tab_ == 1 && cat_ >= 0 ? string(cats_[cat_]) : string(titulosPestana[tab_]);
        Texto(titulo, ox, y, 30, C_PRIM);
        string subtitulo = to_string(n) + " resultados";
        if (!busca_.texto.empty())
            subtitulo += " · \"" + busca_.texto + "\"";
        Texto(subtitulo, ox + 1, y + 40, 13, C_MUTED);

        // Selector de orden (control segmentado)
        const char* opcionesOrden[3] = {"Original", "Menor precio", "Mayor precio"};
        float anchos[3], anchoTotal = 0;
        for (int i = 0; i < 3; i++) {
            anchos[i] = Ancho(opcionesOrden[i], 13) + 26;
            anchoTotal += anchos[i];
        }
        Rectangle segmento = R(ox + cw - anchoTotal - 6, y + 4, anchoTotal + 6, 36);
        Caja(segmento, 0.5f, Al(WHITE, 0.75f), C_LINEA);
        Texto("Ordenar por", segmento.x - Ancho("Ordenar por", 13) - 14, y + 15, 13, C_MUTED);
        float xs = segmento.x + 3;
        for (int i = 0; i < 3; i++) {
            Rectangle r = R(xs, segmento.y + 3, anchos[i], 30);
            bool encima = sobre(r);
            float k = anim(60 + i, encima);
            if (orden_ == i)
                DrawRectangleRounded(r, 0.5f, 10, C_PRIM);
            TextoCentro(opcionesOrden[i], r, 13, orden_ == i ? WHITE : Mezcla(C_MUTED, C_TINTA, k));
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        orden_ = i;
                    });
            }
            xs += anchos[i];
        }
        y += 74;

        // Chip de marca activa
        if (!marca_.empty()) {
            if (chip(20, R(ox, y, Ancho("Marca: " + marca_, 13) + 50, 30), "Marca: " + marca_ + "  ×", true))
                luego([this] {
                    marca_.clear();
                });
            y += 44;
        }

        // Cuadrícula de tarjetas
        int columnasProd = columnas(cw);
        float anchoTarjeta = (cw - (columnasProd - 1) * 16) / columnasProd, altoTarjeta = 262;
        if (n == 0) {
            TextoCentro("Sin resultados · prueba otra palabra o quita el filtro", R(ox, y, cw, 120), 16, C_MUTED);
            y += 120;
        }
        for (int i = 0; i < n; i++) {
            Rectangle r = R(ox + (i % columnasProd) * (anchoTarjeta + 16), y + (i / columnasProd) * (altoTarjeta + 16), anchoTarjeta,
                            altoTarjeta);
            if (r.y + altoTarjeta >= TOP && r.y <= H)  // solo dibuja las tarjetas visibles
                tarjeta(lista[(size_t)i], r, 3000 + i);
        }
        return pie(ox, cw, y + (n + columnasProd - 1) / columnasProd * (altoTarjeta + 16) + 16);
    }

    // ---------- Página: Pedidos ----------
    float pedidos(float ox, float cw, float y) {
        auto& lista = t_.pedidos();
        Texto("Mis pedidos", ox, y, 30, C_PRIM);
        Texto(to_string(lista.size()) + " compras realizadas", ox + 1, y + 40, 13, C_MUTED);
        y += 74;
        if (lista.empty()) {
            Vidrio(R(ox, y, cw, 140), 0.06f, 0.5f);
            TextoCentro("Aún no tienes pedidos · tus comprobantes aparecerán aquí", R(ox, y, cw, 140), 16, C_MUTED);
            return y + 140;
        }
        for (int i = (int)lista.size() - 1; i >= 0; i--) {  // el más reciente primero
            const Pedido& p = lista[(size_t)i];
            Rectangle r = R(ox, y, cw, 118);
            Vidrio(r, 0.08f, 0.5f);
            Texto("N° " + p.numero, r.x + 22, r.y + 16, 18, C_TINTA);
            Texto(p.fecha, r.x + 22 + Ancho("N° " + p.numero, 18) + 14, r.y + 19, 13, C_MUTED);
            Texto(Ajustar(p.cliente + " · " + p.documento, 13, cw - 330), r.x + 22, r.y + 44, 13, C_MUTED);
            string items;
            for (auto& s : p.items)
                items += (items.empty() ? "" : "  |  ") + s;
            Texto(Ajustar(items, 13, cw - 330), r.x + 22, r.y + 66, 13, C_TINTA);
            Texto(Ajustar(p.entrega + " · " + p.pago, 12, cw - 330), r.x + 22, r.y + 90, 12, C_MUTED);
            TextoDer(dinero(p.total), r.x + cw - 24, r.y + 18, 26, C_TINTA);
            TextoDer("Atendió " + p.vendedor, r.x + cw - 24, r.y + 62, 12, C_MUTED);
            TextoDer("Comisión " + dinero(p.comision), r.x + cw - 24, r.y + 82, 12, C_OK);
            if (!p.cupon.empty())
                TextoDer("Cupón " + p.cupon, r.x + cw - 24, r.y + 44, 12, C_ACENTO);
            y += 134;
        }
        return y;
    }

    // ---------- Página: Inventario (alta de productos y reposición de stock) ----------
    float inventario(float ox, float cw, float y) {
        Texto("Administrar productos", ox, y, 30, C_PRIM);
        Texto("Agrega artículos, revisa existencias y repón lo que se haya agotado.", ox + 1, y + 40, 14, C_MUTED);
        y += 72;

        // ---- Formulario "Nuevo producto" ----
        Rectangle form = R(ox, y, cw, 470);
        Vidrio(form, 0.06f, 0.6f);
        Texto("Nuevo producto", form.x + 20, form.y + 16, 20, C_TINTA);
        campo(cProdNombre_, R(form.x + 20, form.y + 68, cw * 0.25f, 44));
        campo(cProdMarca_, R(form.x + cw * 0.25f + 34, form.y + 68, cw * 0.18f, 44));
        campo(cProdPrecio_, R(form.x + cw * 0.44f + 26, form.y + 68, cw * 0.14f, 44));
        campo(cProdStock_, R(form.x + cw * 0.60f + 18, form.y + 68, cw * 0.14f, 44));

        // Marcas del catálogo (paginadas de 6 en 6)
        Texto("Elige una marca del catálogo o escribe otra arriba", form.x + 20, form.y + 130, 12, C_MUTED);
        const int marcasPorPagina = 6;
        int paginasMarcas = max(1, (int)(marcas_.size() + marcasPorPagina - 1) / marcasPorPagina);
        paginaMarcas_ = Clamp(paginaMarcas_, 0, paginasMarcas - 1);
        float marcaX = form.x + 20, marcaY = form.y + 148;
        if (boton(720, R(marcaX, marcaY, 38, 30), "‹", C_SUAVE, paginaMarcas_ > 0, 16))
            luego([this] {
                paginaMarcas_ = max(0, paginaMarcas_ - 1);
            });
        float anchoMarcaChip = (cw - 20 - 38 - 38 - 5 * 6) / 6.0f;
        for (int i = 0; i < marcasPorPagina && paginaMarcas_ * marcasPorPagina + i < (int)marcas_.size(); i++) {
            string marca = marcas_[(size_t)(paginaMarcas_ * marcasPorPagina + i)];
            if (chip(730 + i, R(marcaX + 44 + i * (anchoMarcaChip + 6), marcaY, anchoMarcaChip, 30), Ajustar(marca, 12, anchoMarcaChip - 12),
                     cProdMarca_.texto == marca))
                luego([this, marca] {
                    cProdMarca_.texto = marca;
                    modelosElegidos_.clear();
                    foco_ = nullptr;
                });
        }
        if (boton(721, R(form.x + cw - 58, marcaY, 38, 30), "›", C_SUAVE, paginaMarcas_ + 1 < paginasMarcas, 16))
            luego([this, paginasMarcas] {
                paginaMarcas_ = min(paginasMarcas - 1, paginaMarcas_ + 1);
            });
        Texto(to_string(min((paginaMarcas_ + 1) * marcasPorPagina, (int)marcas_.size())) + "/" + to_string(marcas_.size()) + " marcas",
              form.x + 20, form.y + 182, 10, C_MUTED);

        // Modelos de la marca elegida (selección múltiple)
        Texto("Modelos de " + (cProdMarca_.texto.empty() ? string("la marca") : cProdMarca_.texto) + " (marca uno o varios)",
              form.x + 20, form.y + 204, 12, C_MUTED);
        vector<string> opciones = modelosDeMarca(cProdMarca_.texto);
        float anchoModelo = (cw - 40 - 3 * 8) / 4.0f;
        for (int i = 0; i < (int)opciones.size(); i++) {
            const string& modelo = opciones[(size_t)i];
            bool elegido = find(modelosElegidos_.begin(), modelosElegidos_.end(), modelo) != modelosElegidos_.end();
            Rectangle chipRect = R(form.x + 20 + (i % 4) * (anchoModelo + 8), form.y + 222 + (i / 4) * 40.0f, anchoModelo, 32);
            if (chip(740 + i, chipRect, Ajustar(modelo, 12, anchoModelo - 12), elegido, ac_[catNueva_]))
                luego([this, modelo] {
                    auto it = find(modelosElegidos_.begin(), modelosElegidos_.end(), modelo);
                    if (it == modelosElegidos_.end())
                        modelosElegidos_.push_back(modelo);
                    else
                        modelosElegidos_.erase(it);
                });
        }
        string resumenElegidos;
        for (const auto& modelo : modelosElegidos_)
            resumenElegidos += (resumenElegidos.empty() ? "" : " · ") + modelo;
        Texto(Ajustar("Seleccionados: " + (resumenElegidos.empty() ? string("elige uno o varios modelos") : resumenElegidos), 11, cw - 40),
              form.x + 20, form.y + 302, 11, resumenElegidos.empty() ? C_MUTED : C_PRIM);

        campo(cProdDetalle_, R(form.x + 20, form.y + 326, cw * 0.52f, 44));

        // Categoría
        Texto("Categoría", form.x + 20, form.y + 390, 12, C_MUTED);
        float chipX = form.x + 20;
        for (int i = 0; i < 6; i++) {
            float anchoChip = Ancho(cats_[i], 12) + 20;
            if (chip(710 + i, R(chipX, form.y + 409, anchoChip, 34), cats_[i], catNueva_ == i, ac_[i]))
                luego([this, i] {
                    catNueva_ = i;
                });
            chipX += anchoChip + 6;
        }
        string codigoNuevo = t_.siguienteCodigo();
        Texto("Imagen opcional: img/productos/" + codigoNuevo + ".png", form.x + 20, form.y + 447, 11, C_MUTED);

        // Botón Agregar producto
        if (boton(719, R(form.x + cw - 190, form.y + 405, 170, 42), "Agregar producto", C_PRIM, true, 14)) {
            string nombre = cProdNombre_.texto, marca = cProdMarca_.texto, detalle = cProdDetalle_.texto;
            double precio = atof(cProdPrecio_.texto.c_str());
            int stock = atoi(cProdStock_.texto.c_str());
            if (nombre.empty() || marca.empty() || cProdPrecio_.texto.empty() || precio <= 0 || cProdStock_.texto.empty())
                aviso("Completa nombre, marca, precio y stock inicial");
            else {
                int cantidadModelos = modelosElegidos_.empty() ? 1 : (int)modelosElegidos_.size();
                Ficha ficha{codigoNuevo, nombre, marca, precio, 0, stock, textoModelosElegidos(modelosElegidos_)};
                if (t_.agregarPersonalizado(ficha, catNueva_, detalle.empty() ? "Producto nuevo" : detalle)) {
                    marcas_ = t_.marcas();
                    for (Campo* c : {&cProdNombre_, &cProdMarca_, &cProdPrecio_, &cProdStock_, &cProdDetalle_})
                        c->texto.clear();
                    modelosElegidos_.clear();
                    paginaMarcas_ = 0;
                    aviso("Producto agregado con " + to_string(cantidadModelos) + " opciones · " + nombre);
                } else
                    aviso("No se pudo guardar el producto");
            }
        }
        y += form.height + 20;

        // ---- Lista de stock ----
        auto todos = t_.productos();
        vector<const Producto*> lista;
        string busqueda = minusc(busca_.texto);
        for (const Producto* p : todos)
            if (busqueda.empty() || minusc(p->nombre() + " " + p->marca() + " " + p->codigo()).find(busqueda) != string::npos)
                lista.push_back(p);
        titulo(ox, y, "Stock de la tienda", to_string(lista.size()) + " productos");
        int totalUnidades = 0, agotados = 0;
        for (const Producto* p : todos) {
            totalUnidades += p->stock();
            if (!p->disponible())
                agotados++;
        }
        Texto("Total en tienda: " + to_string(totalUnidades) + " unidades · " + to_string(agotados) + " agotados", ox + 14, y + 34,
              12, C_MUTED);
        cRepo_.pista = "10";
        campo(cRepo_, R(ox + cw - 170, y + 24, 170, 40));  // cantidad que se suma al pulsar "Agregar"
        y += 84;
        int cantidadRepo = atoi(cRepo_.texto.c_str());
        if (lista.empty()) {
            TextoCentro("No hay productos con ese nombre", R(ox, y, cw, 90), 15, C_MUTED);
            return y + 100;
        }
        const float altoFila = 62;
        for (int i = 0; i < (int)lista.size(); i++) {
            const Producto* p = lista[(size_t)i];
            Rectangle r = R(ox, y + i * (altoFila + 8), cw, altoFila);
            if (r.y + altoFila < TOP || r.y > H)
                continue;  // fuera de pantalla
            Vidrio(r, 0.08f, 0.32f);
            Texto(Ajustar(p->nombre(), 15, cw * 0.40f), r.x + 16, r.y + 10, 15, C_TINTA);
            Texto(Ajustar(p->codigo() + " · " + cats_[p->categoria()], 11, cw * 0.40f), r.x + 16, r.y + 34, 11, C_MUTED);
            tag(R(r.x + cw - 342, r.y + 20, 116, 22), p->estado() + " · " + to_string(p->stock()), estadoColor(p));
            string codigo = p->codigo();
            if (boton(8000 + i * 2, R(r.x + cw - 216, r.y + 13, 54, 36), "+1", C_SUAVE, true, 14))
                luego([this, codigo] {
                    t_.reponerStock(codigo, 1);
                    aviso("Se agregó una unidad al stock");
                });
            if (boton(8001 + i * 2, R(r.x + cw - 154, r.y + 13, 138, 36), "Agregar " + to_string(max(cantidadRepo, 0)), C_PRIM,
                      cantidadRepo > 0, 14))
                luego([this, codigo, cantidadRepo] {
                    t_.reponerStock(codigo, cantidadRepo);
                    aviso("Se agregaron " + to_string(cantidadRepo) + " unidades al stock");
                });
        }
        return y + (lista.size() * (altoFila + 8));
    }

    // ---------- Página: Nosotros ----------
    float nosotros(float ox, float cw, float y) {
        Texto("Sobre TecnoStore", ox, y, 30, C_PRIM);
        y += 56;
        Rectangle r = R(ox, y, cw, 96);
        Vidrio(r, 0.08f, 0.5f);
        Texto("Tienda de computadoras: hardware, accesorios, periféricos, software, equipos completos y servicio técnico.",
              r.x + 24, r.y + 18, 16, C_TINTA);
        Texto("Horario: Lun a Sáb 9:00 - 20:00  ·  Tel. 951 000 000  ·  ventas@tecnostore.pe  ·  Pagos: Efectivo, Tarjeta, Yape y Plin",
              r.x + 24, r.y + 54, 13, C_MUTED);
        y += 126;

        // Equipo
        Texto("Nuestro equipo", ox, y, 22, C_TINTA);
        y += 42;
        auto& equipo = t_.equipo();
        int cantidad = (int)equipo.size(), columnasEquipo = max(2, min(cantidad, (int)((cw + 16) / 236)));
        float anchoTarjeta = (cw - (columnasEquipo - 1) * 16) / columnasEquipo;
        for (int i = 0; i < cantidad; i++) {
            Rectangle c = R(ox + (i % columnasEquipo) * (anchoTarjeta + 16), y + (i / columnasEquipo) * 120.0f, anchoTarjeta, 104);
            Vidrio(c, 0.1f, 0.4f);
            DrawCircleV(Vector2{c.x + 34, c.y + 38}, 20, Al(ac_[i % 6], 0.25f));
            TextoCentro(equipo[(size_t)i]->datos().nombres.substr(0, 1), R(c.x + 14, c.y + 18, 40, 40), 18, C_TINTA);
            Texto(Ajustar(equipo[(size_t)i]->nombreCompleto(), 14, anchoTarjeta - 76), c.x + 64, c.y + 22, 14, C_TINTA);
            Texto(Ajustar(equipo[(size_t)i]->codigo(), 12, anchoTarjeta - 76), c.x + 64, c.y + 44, 12, C_MUTED);
            Texto(Ajustar(equipo[(size_t)i]->rol(), 12, anchoTarjeta - 28), c.x + 16, c.y + 76, 12, ac_[i % 6]);
        }
        y += (cantidad + columnasEquipo - 1) / columnasEquipo * 120.0f + 18;

        // Proveedores
        Texto("Proveedores", ox, y, 22, C_TINTA);
        y += 42;
        auto& proveedores = t_.proveedores();
        float anchoProv = (cw - 16) / 2;
        for (int i = 0; i < (int)proveedores.size(); i++) {
            Rectangle c = R(ox + i * (anchoProv + 16), y, anchoProv, 80);
            Vidrio(c, 0.1f, 0.4f);
            Texto(proveedores[(size_t)i]->razon(), c.x + 20, c.y + 16, 16, C_TINTA);
            Texto(Ajustar(proveedores[(size_t)i]->detalle() + " · " + proveedores[(size_t)i]->rol(), 12, anchoProv - 40), c.x + 20,
                  c.y + 44, 12, C_MUTED);
        }
        return y + 80;
    }

    // Dibuja la pestaña actual y la barra de desplazamiento de la página
    void dibujarPagina() {
        sobreBarra_ = false;
        M = (capa_ == 0 && CheckCollisionPointRec(mouse_, R(0, TOP, W, H - TOP))) ? mouse_ : OFF;
        const float ox = 48, cw = W - 96, y0 = TOP + 20 - scroll_;
        float yFinal = tab_ == 0   ? inicio(ox, cw, y0)
                       : tab_ <= 3 ? catalogo(ox, cw, y0)
                       : tab_ == 4 ? pedidos(ox, cw, y0)
                       : tab_ == 5 ? nosotros(ox, cw, y0)
                                   : inventario(ox, cw, y0);
        contentH_ = yFinal - y0 + 40;
        float altoVisible = H - TOP, maxScroll = max(0.0f, contentH_ - altoVisible);
        if (maxScroll > 0) {
            float altoBarra = max(40.0f, altoVisible * altoVisible / contentH_);
            DrawRectangleRounded(R(W - 9, TOP + 4 + scroll_ / maxScroll * (altoVisible - 8 - altoBarra), 4, altoBarra), 1.0f, 6,
                                 Al(C_TINTA, 0.22f));
        }
    }

    // =============== barra superior ===============
    void dibujarTopbar() {
        M = capa_ == 0 ? mouse_ : OFF;
        DrawRectangleGradientV(0, (int)TOP, (int)W + 1, 12, Color{60, 80, 150, 26}, Color{60, 80, 150, 0});
        DrawRectangle(0, 0, (int)W + 1, (int)TOP, Al(WHITE, 0.94f));
        DrawRectangle(0, (int)TOP - 3, (int)W + 1, 3, C_PRIM);

        // Logo
        Rectangle logo = R(24, 12, 40, 40);
        DrawRectangleRounded(R(logo.x + 2, logo.y + 4, logo.width - 4, logo.height - 2), 0.3f, 10, Al(C_PRIM, 0.25f));
        DrawRectangleRounded(logo, 0.3f, 10, C_PRIM);
        TextoCentro("T", logo, 22, WHITE);
        Texto("TecnoStore", 74, 12, 20, C_TINTA);
        Texto("Tienda de computadoras", 74, 37, 11, C_MUTED);

        // Pestañas
        float x = 230;
        for (int i = 0; i < 7; i++) {
            float ancho = Ancho(tabs_[i], 15) + 24;
            Rectangle r = R(x, 14, ancho, 36);
            bool encima = sobre(r);
            float k = anim(40 + i, encima);
            if (tab_ == i)
                DrawRectangleRounded(r, 0.5f, 10, C_PRIM);
            else if (k > 0.02f)
                DrawRectangleRounded(r, 0.5f, 10, Al(C_SUAVE, k));
            TextoCentro(tabs_[i], r, 15, tab_ == i ? WHITE : Mezcla(C_MUTED, C_TINTA, k));
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        filtro(i, -1, "", "");
                        busca_.texto.clear();
                    });
            }
            x += ancho + 4;
        }

        // Buscador
        busca_.pista = "Buscar producto o marca...";
        campo(busca_, R(W - 476, 12, 240, 40));

        // Botón del carrito
        Rectangle botonCarrito = R(W - 216, 12, 192, 40);
        bool encima = sobre(botonCarrito);
        float k = anim(50, encima);
        int cantidad = t_.carrito().cantidad();
        DrawRectangleRounded(R(botonCarrito.x + 3, botonCarrito.y + 5, botonCarrito.width - 6, botonCarrito.height - 3), 0.5f, 12,
                             Al(C_PRIM, 0.28f));
        DrawRectangleRounded(botonCarrito, 0.5f, 12, Mezcla(C_PRIM, WHITE, k * 0.15f));
        Texto(cantidad > 0 ? dinero(t_.carrito().subtotal() - t_.carrito().descuentos()) : string("Mi carrito"), botonCarrito.x + 20,
              botonCarrito.y + 11, 15, WHITE);
        DrawCircleV(Vector2{botonCarrito.x + botonCarrito.width - 24, botonCarrito.y + 20}, 12 * (1 + bump_ * 0.3f), C_ACENTO);
        TextoCentro(to_string(cantidad), R(botonCarrito.x + botonCarrito.width - 38, botonCarrito.y + 8, 28, 24), 13, C_PRIM);
        if (encima) {
            manito_ = true;
            if (click_)
                ir(1);
        }
    }

    // =============== carrito lateral ===============
    void dibujarCarrito() {
        float alfa = capaA_[1];
        if (alfa < 0.01f)
            return;
        M = capa_ == 1 ? mouse_ : OFF;
        float suave = 1 - powf(1 - alfa, 3), anchoPanel = 400, dx = W - anchoPanel * suave;  // desliza desde la derecha
        velo(alfa, 0.35f);
        Rectangle panel = R(dx, 0, anchoPanel, H);
        DrawRectangle((int)dx, 0, (int)anchoPanel + 1, (int)H + 1, C_MODAL);
        DrawLine((int)dx, 0, (int)dx, (int)H, C_LINEA);
        if (capa_ == 1 && click_ && !sobre(panel))
            ir(0);

        Carrito& carrito = t_.carrito();
        int numLineas = (int)carrito.lineas().size();
        Texto("Tu carrito", dx + 20, 18, 26, C_TINTA);
        if (equis(R(dx + anchoPanel - 48, 20, 28, 28)))
            ir(0);

        if (numLineas > 0) {
            Rectangle vaciar = R(dx + 20, 54, 80, 18);
            bool encima = sobre(vaciar);
            Texto("Vaciar todo", vaciar.x, vaciar.y, 12, encima ? C_PELIGRO : C_MUTED);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this] {
                        t_.carrito().vaciar();
                        scrollCar_ = 0;
                    });
            }
        } else {
            Texto("Tu carrito está vacío", dx + 20, 120, 16, C_TINTA);
            Texto("Elige un producto y su modelo para agregarlo", dx + 20, 144, 12, C_MUTED);
        }

        // Lista (máximo 5 líneas visibles, con rueda del mouse)
        const int lineasVisibles = 5;
        if (sobre(R(dx + 20, 76, 360, 290)) && wheel_ != 0)
            scrollCar_ += wheel_ < 0 ? 1 : -1;
        scrollCar_ = (int)Clamp((float)scrollCar_, 0, (float)max(0, numLineas - lineasVisibles));
        for (int i = 0; i < min(numLineas - scrollCar_, lineasVisibles); i++) {
            int indice = scrollCar_ + i;
            const Linea linea = carrito.lineas()[(size_t)indice];
            const Producto* producto = linea.producto;
            int modelo = linea.modelo;
            Rectangle fila = R(dx + 20, 76.0f + i * 58, 360, 54);
            if (sobre(fila))
                DrawRectangleRounded(fila, 0.2f, 8, C_SUAVE);
            Rectangle icono = R(fila.x + 8, fila.y + 8, 38, 38);
            DrawRectangleRounded(icono, 0.3f, 8, Al(ac_[producto->categoria()], 0.18f));
            TextoCentro(producto->nombre().substr(0, 1), icono, 17, ac_[producto->categoria()]);
            Texto(Ajustar(producto->nombre(), 15, 190), fila.x + 56, fila.y + 8, 15, C_TINTA);
            Texto(Ajustar(producto->modelos()[(size_t)modelo].nombre + " · " + dinero(producto->precioFinal(modelo) * linea.cantidad), 12,
                          196),
                  fila.x + 56, fila.y + 30, 12, C_MUTED);

            // Botones − y +
            Rectangle botonMenos = R(fila.x + 262, fila.y + 15, 24, 24), botonMas = R(fila.x + 314, fila.y + 15, 24, 24);
            bool encimaMenos = sobre(botonMenos), encimaMas = sobre(botonMas);
            DrawRectangleRounded(botonMenos, 1.0f, 10, encimaMenos ? Mezcla(C_LINEA, C_PELIGRO, 0.35f) : C_LINEA);
            DrawLineEx(Vector2{botonMenos.x + 7, botonMenos.y + 12}, Vector2{botonMenos.x + 17, botonMenos.y + 12}, 2,
                       encimaMenos ? C_PELIGRO : C_MUTED);
            DrawRectangleRounded(botonMas, 1.0f, 10, Mezcla(C_SUAVE, C_PRIM, encimaMas ? 0.6f : 0.3f));
            DrawLineEx(Vector2{botonMas.x + 7, botonMas.y + 12}, Vector2{botonMas.x + 17, botonMas.y + 12}, 2, C_TINTA);
            DrawLineEx(Vector2{botonMas.x + 12, botonMas.y + 7}, Vector2{botonMas.x + 12, botonMas.y + 17}, 2, C_TINTA);
            TextoCentro(to_string(linea.cantidad), R(fila.x + 286, fila.y + 15, 28, 24), 14, C_TINTA);
            if (encimaMenos) {
                manito_ = true;
                if (click_)
                    luego([this, indice] {
                        t_.carrito().quitar((size_t)indice);
                    });
            }
            if (encimaMas) {
                manito_ = true;
                if (click_)
                    luego([this, producto, modelo] {
                        if (t_.carrito().agregar(producto, modelo))
                            bump_ = 1;
                        else
                            aviso("No hay más stock de " + producto->nombre());
                    });
            }
        }
        if (numLineas > lineasVisibles)
            Texto("Rueda del mouse para ver más", dx + 20, 358, 11, C_PRIM);

        // Totales
        DrawLine((int)dx + 20, 380, (int)dx + 380, 380, C_LINEA);
        float yy = 394;
        Texto("Subtotal", dx + 20, yy, 14, C_MUTED);
        TextoDer(dinero(carrito.subtotal()), dx + 380, yy, 14, C_TINTA);
        yy += 26;
        for (auto& descuento : carrito.detalleDescuentos()) {
            Texto(Ajustar(descuento.first, 13, 250), dx + 20, yy, 13, C_OK);
            TextoDer("-" + dinero(descuento.second), dx + 380, yy, 13, C_OK);
            yy += 22;
        }
        Texto("Total parcial", dx + 20, 520, 14, C_MUTED);
        TextoDer(dinero(carrito.subtotal() - carrito.descuentos()), dx + 380, 508, 28, C_TINTA);
        Texto("El envío, el cupón y el pago se eligen en el siguiente paso", dx + 20, 556, 12, C_MUTED);
        if (boton(80, R(dx + 20, H - 84, 360, 52), "Continuar compra", C_PRIM, numLineas > 0, 17)) {
            paso_ = 0;
            foco_ = nullptr;
            ir(2);
        }
    }

    // =============== checkout: datos, entrega, pago, listo ===============

    // Campos del paso actual (para moverse con la tecla Tab)
    vector<Campo*> camposPaso() {
        if (paso_ == 0)
            return {&cNom_, &cApP_, &cApM_, &cDni_, &cTel_, &cMail_};
        vector<Campo*> campos;
        if (paso_ == 1) {
            if (t_.comprobanteActual().pideRUC()) {
                campos.push_back(&cRuc_);
                campos.push_back(&cRazon_);
            }
            if (t_.envioActual().pideDireccion())
                campos.push_back(&cDir_);
        } else if (paso_ == 2) {
            campos.push_back(&cPago_);
            campos.push_back(&cCupon_);
        }
        return campos;
    }

    // Valida los campos del paso actual y marca los errores
    bool validar() {
        bool todoBien = true;
        auto comprobar = [&](Campo& campo, bool condicion, const char* mensaje) {
            campo.error = condicion ? "" : mensaje;
            if (!condicion)
                todoBien = false;
        };
        if (paso_ == 0) {
            comprobar(cNom_, cNom_.texto.size() >= 2, "Escribe tus nombres");
            comprobar(cApP_, cApP_.texto.size() >= 2, "Escribe tu apellido paterno");
            comprobar(cApM_, cApM_.texto.size() >= 2, "Escribe tu apellido materno");
            comprobar(cDni_, cDni_.texto.size() == 8, "El DNI tiene 8 dígitos");
            comprobar(cTel_, cTel_.texto.size() == 9 && cTel_.texto[0] == '9', "9 dígitos, empieza con 9");
            size_t arroba = cMail_.texto.find('@');
            comprobar(cMail_, arroba != string::npos && arroba > 0 && cMail_.texto.find('.', arroba) != string::npos, "Correo no válido");
        } else if (paso_ == 1) {
            cRuc_.error = cRazon_.error = cDir_.error = "";
            if (t_.comprobanteActual().pideRUC()) {
                bool rucValido = cRuc_.texto.size() == 11 && (cRuc_.texto.rfind("10", 0) == 0 || cRuc_.texto.rfind("20", 0) == 0);
                comprobar(cRuc_, rucValido, "RUC de 11 dígitos (10 o 20)");
                comprobar(cRazon_, cRazon_.texto.size() >= 3, "Escribe la razón social");
            }
            if (t_.envioActual().pideDireccion())
                comprobar(cDir_, cDir_.texto.size() >= 6, "Escribe tu dirección");
        } else {
            cPago_.error = t_.medioActual().validar(cPago_.texto, t_.total());
            todoBien = cPago_.error.empty();
        }
        return todoBien;
    }

    // Pasa al siguiente paso; en el último paso realiza el pago
    void avanzar() {
        if (t_.carrito().cantidad() == 0 || !validar())
            return;
        foco_ = nullptr;
        if (paso_ < 2) {
            paso_++;
            return;
        }
        Cliente cliente(DatosPersona{cNom_.texto, cApP_.texto, cApM_.texto, cTel_.texto, cMail_.texto, cDni_.texto},
                        t_.comprobanteActual().pideRUC() ? "empresa" : "persona", cDir_.texto);
        ultimo_ = t_.pagar(cliente, cRuc_.texto, cRazon_.texto, cDir_.texto, cPago_.texto, vend_);
        cPago_.texto.clear();
        cCupon_.texto.clear();
        cCupon_.error.clear();
        paso_ = 3;
    }

    void dibujarCheckout() {
        float alfa = capaA_[2];
        if (alfa < 0.01f)
            return;
        M = capa_ == 2 ? mouse_ : OFF;
        float suave = 1 - powf(1 - alfa, 3);
        velo(alfa, 0.40f);
        Rectangle P = R((W - 940) / 2, (H - 600) / 2 + (1 - suave) * 34, 940, 600);
        if (capa_ == 2 && click_ && !sobre(P))
            ir(0);
        Vidrio(P, 0.03f, 3, C_MODAL);
        float x0 = P.x + 32;
        Texto("Finalizar compra", x0, P.y + 22, 24, C_TINTA);

        // Indicador de pasos
        const char* nombresPasos[4] = {"Datos", "Entrega", "Pago", "Listo"};
        for (int i = 0; i < 4; i++) {
            float sx = x0 + i * 130;
            if (i < 3)
                DrawRectangle((int)(sx + 88), (int)(P.y + 74), 32, 2, i < paso_ ? C_OK : C_LINEA);
            DrawCircleV(Vector2{sx + 11, P.y + 74}, 11, i < paso_ || paso_ == 3 ? C_OK : i == paso_ ? C_PRIM : C_LINEA);
            TextoCentro(to_string(i + 1), R(sx, P.y + 63, 22, 22), 12, WHITE);
            Texto(nombresPasos[i], sx + 30, P.y + 65, 14, i <= paso_ ? C_TINTA : C_MUTED);
        }

        auto fila = [&](int n) {
            return P.y + 158 + n * 82;
        };
        // Grupo de chips (opciones) que ejecuta "accion(indice)" al elegir una
        auto grupoChips = [&](int idBase, float y, const vector<string>& opciones, int elegida, int columnasChips,
                              function<void(int)> accion) {
            for (int i = 0; i < (int)opciones.size(); i++)
                if (chip(idBase + i, R(x0 + (i % columnasChips) * 276.0f, y + (i / columnasChips) * 48.0f, 264, 40),
                         opciones[(size_t)i], i == elegida)) {
                    int j = i;
                    luego([accion, j] {
                        accion(j);
                    });
                }
        };
        auto aplicarCupon = [&] {
            cCupon_.error = t_.aplicarCupon(cCupon_.texto);
            if (cCupon_.error.empty()) {
                aviso("Cupón aplicado: " + t_.cupon()->codigo());
                cCupon_.texto.clear();
                foco_ = nullptr;
            }
        };

        // ---- Contenido de cada paso ----
        if (paso_ == 0) {  // DATOS
            Texto("Para emitir tu comprobante necesitamos tus datos", x0, P.y + 108, 15, C_MUTED);
            campo(cNom_, R(x0, fila(0), 264, 46));
            campo(cApP_, R(x0 + 276, fila(0), 264, 46));
            campo(cApM_, R(x0, fila(1), 264, 46));
            campo(cDni_, R(x0 + 276, fila(1), 264, 46));
            campo(cTel_, R(x0, fila(2), 264, 46));
            campo(cMail_, R(x0 + 276, fila(2), 264, 46));
        } else if (paso_ == 1) {  // ENTREGA
            Texto("COMPROBANTE", x0, P.y + 108, 11, C_MUTED);
            grupoChips(100, P.y + 128, t_.nombresComprobantes(), t_.comprobante(), 2, [this](int i) {
                t_.elegirComprobante(i);
            });
            if (t_.comprobanteActual().pideRUC()) {
                campo(cRuc_, R(x0, P.y + 226, 264, 46));
                campo(cRazon_, R(x0 + 276, P.y + 226, 264, 46));
            } else
                Texto("La boleta se emite con el DNI que ingresaste", x0, P.y + 184, 13, C_MUTED);
            Texto("ENTREGA", x0, P.y + 296, 11, C_MUTED);
            grupoChips(110, P.y + 316, t_.nombresEnvios(), t_.envio(), 2, [this](int i) {
                t_.elegirEnvio(i);
            });
            if (t_.envioActual().pideDireccion())
                campo(cDir_, R(x0, P.y + 404, 540, 46));
            else
                Texto("Recoge tu pedido en tienda · Lun a Sáb 9:00 - 20:00", x0, P.y + 386, 13, C_MUTED);
        } else if (paso_ == 2) {  // PAGO
            Texto("ATENDIDO POR", x0, P.y + 108, 11, C_MUTED);
            vector<string> nombresVendedores;
            for (auto* v : t_.vendedores())
                nombresVendedores.push_back(v->nombreCompleto());
            grupoChips(120, P.y + 128, nombresVendedores, vend_, 2, [this](int i) {
                vend_ = i;
            });
            Texto("MEDIO DE PAGO", x0, P.y + 196, 11, C_MUTED);
            grupoChips(130, P.y + 216, t_.nombresMedios(), t_.medio(), 2, [this](int i) {
                t_.elegirMedio(i);
                cPago_.error.clear();
            });
            cPago_.etiqueta = t_.medioActual().campo();
            campo(cPago_, R(x0, P.y + 346, 540, 46));
            if (cPago_.error.empty() && t_.medioActual().validar(cPago_.texto, t_.total()).empty())
                Texto(Ajustar(t_.medioActual().resumen(cPago_.texto, t_.total()), 13, 540), x0 + 2, P.y + 404, 13, C_OK);

            // Cupón de descuento
            if (t_.cupon()) {
                Rectangle cuponRect = R(x0, P.y + 452, 540, 40);
                Caja(cuponRect, 0.2f, Al(C_OK, 0.12f), C_OK);
                Texto("CUPÓN DE DESCUENTO", x0 + 2, P.y + 432, 11, C_MUTED);
                Texto(Ajustar(t_.cupon()->codigo() + " · " + t_.cupon()->descripcion() + " · -" + dinero(t_.descuentoCupon()), 13, 400),
                      cuponRect.x + 14, cuponRect.y + 12, 13, C_OK);
                if (boton(85, R(cuponRect.x + cuponRect.width - 88, cuponRect.y + 8, 76, 24), "Quitar", C_SUAVE, true, 12))
                    luego([this] {
                        t_.quitarCupon();
                    });
            } else {
                cCupon_.pista = "Ej: TECNO10";
                campo(cCupon_, R(x0, P.y + 452, 380, 40));
                if (boton(84, R(x0 + 392, P.y + 452, 148, 40), "Aplicar", C_PRIM, true, 15) || (enter_ && foco_ == &cCupon_))
                    aplicarCupon();
            }
        } else {  // LISTO
            float escala = 0.6f + 0.4f * suave;
            Vector2 centro = {x0 + 270, P.y + 190};
            DrawCircleV(centro, 46 * escala, Al(C_OK, 0.2f));
            DrawCircleV(centro, 34 * escala, C_OK);
            DrawLineEx(Vector2{centro.x - 14 * escala, centro.y}, Vector2{centro.x - 4 * escala, centro.y + 11 * escala}, 4, WHITE);
            DrawLineEx(Vector2{centro.x - 4 * escala, centro.y + 11 * escala}, Vector2{centro.x + 16 * escala, centro.y - 12 * escala}, 4,
                       WHITE);
            TextoCentro("¡Compra realizada!", R(x0, P.y + 260, 540, 40), 30, C_TINTA);
            TextoCentro("Comprobante " + ultimo_.numero, R(x0, P.y + 308, 540, 24), 15, C_MUTED);
            TextoCentro(Ajustar(ultimo_.comprobante, 14, 540), R(x0, P.y + 336, 540, 24), 14, C_TINTA);
            TextoCentro(Ajustar("Enviaremos el detalle a " + cMail_.texto, 13, 540), R(x0, P.y + 366, 540, 24), 13, C_MUTED);
        }

        // ---- Botones inferiores ----
        if (paso_ == 3) {
            if (boton(83, R(x0, P.y + 536, 540, 44), "Seguir comprando", C_PRIM, true, 16)) {
                paso_ = 0;
                ir(0);
            }
        } else {
            if (paso_ > 0 && boton(81, R(x0, P.y + 536, 120, 44), "Atrás", C_SUAVE)) {
                paso_--;
                foco_ = nullptr;
            }
            string textoBoton = paso_ < 2 ? "Continuar" : "Pagar " + dinero(t_.total());
            Rectangle botonAvanzar = R(paso_ > 0 ? x0 + 132 : x0, P.y + 536, paso_ > 0 ? 408 : 540, 44);
            if (boton(82, botonAvanzar, textoBoton, paso_ == 2 ? C_ACENTO : C_PRIM, true, 17) || (enter_ && foco_ != &cCupon_))
                avanzar();
        }

        // ---- Panel derecho: resumen / comprobante (azul marino con acentos naranja) ----
        Rectangle panelDer = R(P.x + 596, P.y + 16, 328, P.height - 32);
        DrawRectangleRounded(panelDer, 0.04f, 12, C_PRIM);
        const Color texto1 = WHITE, texto2 = C_LINEA, verde{110, 225, 160, 255};
        if (equis(R(panelDer.x + panelDer.width - 44, panelDer.y + 12, 28, 28)))
            ir(0);
        Texto(paso_ == 3 ? "Comprobante" : "Resumen", panelDer.x + 20, panelDer.y + 16, 18, texto1);
        DrawRectangleRounded(R(panelDer.x + 20, panelDer.y + 44, 36, 3), 1.0f, 4, C_ACENTO);
        float yy = panelDer.y + 64, xDerecha = panelDer.x + panelDer.width - 20;

        if (paso_ < 3) {  // resumen de la compra
            auto& lineas = t_.carrito().lineas();
            for (int i = 0; i < min((int)lineas.size(), 6); i++, yy += 26) {
                const Linea& l = lineas[(size_t)i];
                Texto(Ajustar(to_string(l.cantidad) + "× " + l.producto->nombre() + " " + l.producto->modelos()[(size_t)l.modelo].nombre,
                              13, 196),
                      panelDer.x + 20, yy, 13, texto1);
                TextoDer(dinero(l.producto->precioFinal(l.modelo) * l.cantidad), xDerecha, yy, 13, texto2);
            }
            if (lineas.size() > 6)
                Texto("y " + to_string(lineas.size() - 6) + " más..", panelDer.x + 20, yy, 12, texto2);

            float yDesglose = panelDer.y + panelDer.height - 202;
            DrawLine((int)panelDer.x + 20, (int)yDesglose - 10, (int)xDerecha, (int)yDesglose - 10, Al(WHITE, 0.25f));
            auto filaResumen = [&](const string& etiqueta, const string& valor, Color color) {
                Texto(etiqueta, panelDer.x + 20, yDesglose, 14, color);
                TextoDer(valor, xDerecha, yDesglose, 14, color);
                yDesglose += 25;
            };
            filaResumen("Subtotal", dinero(t_.carrito().subtotal()), texto2);
            if (t_.carrito().descuentos() > 0)
                filaResumen("Descuentos", "-" + dinero(t_.carrito().descuentos()), verde);
            if (t_.descuentoCupon() > 0)
                filaResumen("Cupón " + t_.cupon()->codigo(), "-" + dinero(t_.descuentoCupon()), verde);
            filaResumen("Envío", t_.envioActual().costo() > 0 ? dinero(t_.envioActual().costo()) : string("Gratis"), texto2);
            if (t_.recargo() > 0)
                filaResumen("Recargo tarjeta", dinero(t_.recargo()), texto2);
            Texto("Total", panelDer.x + 20, panelDer.y + panelDer.height - 52, 15, texto2);
            TextoDer(dinero(t_.total()), xDerecha, panelDer.y + panelDer.height - 62, 28, C_ACENTO);
        } else {  // comprobante del pedido pagado
            vector<string> datos = {"N° " + ultimo_.numero + " · " + ultimo_.fecha,
                                    ultimo_.cliente,
                                    ultimo_.documento,
                                    ultimo_.entrega,
                                    "Atendió " + ultimo_.vendedor,
                                    ultimo_.pago};
            if (!ultimo_.cupon.empty())
                datos.push_back("Cupón " + ultimo_.cupon);
            for (const string& s : datos) {
                Texto(Ajustar(s, 13, 288), panelDer.x + 20, yy, 13, texto2);
                yy += 24;
            }
            yy += 10;
            DrawLine((int)panelDer.x + 20, (int)yy, (int)xDerecha, (int)yy, Al(WHITE, 0.25f));
            yy += 14;
            for (int i = 0; i < min((int)ultimo_.items.size(), 4); i++, yy += 24)
                Texto(Ajustar(ultimo_.items[(size_t)i], 13, 288), panelDer.x + 20, yy, 13, texto1);
            Texto("Total pagado", panelDer.x + 20, panelDer.y + panelDer.height - 52, 15, texto2);
            TextoDer(dinero(ultimo_.total), xDerecha, panelDer.y + panelDer.height - 62, 28, C_ACENTO);
        }
    }

    // =============== ventana de detalle de producto ===============
    void dibujarProducto() {
        float alfa = capaA_[3];
        if (alfa < 0.01f || !sel_)
            return;
        M = capa_ == 3 ? mouse_ : OFF;
        const Producto* p = sel_;
        Color ac = ac_[p->categoria()];
        float suave = 1 - powf(1 - alfa, 3);
        velo(alfa, 0.40f);
        Rectangle ventana = R((W - 820) / 2, (H - 530) / 2 + (1 - suave) * 34, 820, 530);
        if (capa_ == 3 && click_ && !sobre(ventana))
            ir(0);
        Vidrio(ventana, 0.04f, 3, C_MODAL);

        // ---- Escenario izquierdo (ilustración con degradado) ----
        Rectangle escenario = R(ventana.x + 14, ventana.y + 14, 340, ventana.height - 28);
        Deg(escenario, 16, Mezcla(ac, Color{255, 70, 150, 255}, 0.35f), Mezcla(ac, Color{90, 60, 200, 255}, 0.55f), false);
        for (int k = 0; k < 4; k++)
            DrawRing(Vector2{escenario.x + 50 + k * 80.0f, escenario.y + 110 + sinf((float)GetTime() * 0.8f + k * 1.7f) * 26},
                     (14.0f + k * 6) * 0.55f, 14.0f + k * 6, 0, 360, 32, Al(WHITE, 0.10f));
        Marca(p->marca(), R(escenario.x + 20, escenario.y + 20, 110, 36));
        if (p->enOferta()) {
            DrawRectangleRounded(R(escenario.x + escenario.width - 96, escenario.y + 26, 76, 22), 0.5f, 8, Al(WHITE, 0.25f));
            TextoCentro("OFERTA", R(escenario.x + escenario.width - 96, escenario.y + 26, 76, 22), 11, WHITE);
        }
        arte(p, R(escenario.x + 10, escenario.y + 70, escenario.width - 20, 330), Mezcla(ac, WHITE, 0.5f));

        // ---- Columna derecha: datos ----
        float x0 = ventana.x + 378, anchoDer = ventana.width - 378 - 28;
        if (equis(R(ventana.x + ventana.width - 46, ventana.y + 14, 28, 28)))
            ir(0);
        Texto(cats_[p->categoria()], x0, ventana.y + 22, 11, ac);
        Texto(Ajustar(p->nombre(), 30, anchoDer - 40), x0, ventana.y + 40, 30, C_TINTA);
        Texto(Ajustar(p->detalle(), 13, anchoDer), x0 + 1, ventana.y + 82, 13, C_MUTED);
        Texto("Código " + p->codigo() + " · " +
                  (p->garantia() > 0 ? string(TextFormat("Garantía %d meses", p->garantia())) : string("Sin garantía extendida")),
              x0 + 1, ventana.y + 104, 13, C_MUTED);
        DrawCircleV(Vector2{x0 + 6, ventana.y + 134}, 4, estadoColor(p));
        Texto(p->estado() + " · " + to_string(p->stock()) + " en stock", x0 + 18, ventana.y + 126, 13, C_TINTA);

        // ---- Selector de modelo: lista con buscador y rueda del mouse ----
        vector<int> indices;  // modelos que coinciden con la búsqueda
        string busqueda = minusc(cModelo_.texto);
        for (int i = 0; i < (int)p->modelos().size(); i++)
            if (busqueda.empty() || minusc(p->modelos()[(size_t)i].nombre).find(busqueda) != string::npos)
                indices.push_back(i);
        const int filasVisibles = 5, numModelos = (int)indices.size();
        Texto("ELIGE TU MODELO", x0, ventana.y + 158, 11, C_MUTED);
        Texto(to_string(p->modelos().size()) + " disponibles", x0 + Ancho("ELIGE TU MODELO", 11) + 10, ventana.y + 158, 11, C_PRIM);
        cModelo_.pista = "Buscar modelo...";
        campo(cModelo_, R(x0 + anchoDer - 210, ventana.y + 146, 210, 34));
        if (cModelo_.texto != prevModelo_) {
            prevModelo_ = cModelo_.texto;
            modeloOff_ = 0;
        }
        Rectangle zonaLista = R(x0, ventana.y + 188, anchoDer, filasVisibles * 44.0f);
        if (sobre(zonaLista) && wheel_ != 0)
            modeloOff_ += wheel_ < 0 ? 1 : -1;
        modeloOff_ = (int)Clamp((float)modeloOff_, 0, (float)max(0, numModelos - filasVisibles));
        if (numModelos == 0)
            TextoCentro("Ningún modelo coincide con la búsqueda", zonaLista, 14, C_MUTED);

        for (int j = 0; j < min(filasVisibles, numModelos - modeloOff_); j++) {
            int i = indices[(size_t)(modeloOff_ + j)];
            const Modelo& modelo = p->modelos()[(size_t)i];
            Rectangle fila = R(x0, ventana.y + 188 + j * 44.0f, anchoDer - 14, 40);
            bool seleccionado = i == modeloSel_, encima = sobre(fila);
            float k = anim(5000 + j, encima);
            Caja(fila, 0.2f, seleccionado ? C_PRIM : Mezcla(Al(WHITE, 0.8f), C_SUAVE, k),
                 seleccionado ? C_PRIM : Mezcla(C_LINEA, C_MUTED, k * 0.5f));
            Vector2 radio = {fila.x + 20, fila.y + 20};  // botón de radio
            DrawCircleV(radio, 8, seleccionado ? WHITE : C_MUTED);
            DrawCircleV(radio, 6, seleccionado ? C_PRIM : WHITE);
            if (seleccionado)
                DrawCircleV(radio, 3.5f, C_ACENTO);
            string precioTexto = dinero(p->precioFinal(i));
            float anchoPrecio = Ancho(precioTexto, 16);
            Texto(Ajustar(modelo.nombre, 15, fila.width - anchoPrecio - (modelo.recomendado ? 230 : 90)), fila.x + 40, fila.y + 11, 15,
                  seleccionado ? WHITE : C_TINTA);
            if (modelo.recomendado)
                tag(R(fila.x + fila.width - 14 - anchoPrecio - 110, fila.y + 11, 98, 18), "RECOMENDADO",
                    seleccionado ? Color{110, 225, 160, 255} : C_OK);
            TextoDer(precioTexto, fila.x + fila.width - 14, fila.y + 11, 16, seleccionado ? WHITE : C_TINTA);
            if (encima) {
                manito_ = true;
                if (click_)
                    luego([this, i] {
                        modeloSel_ = i;
                    });
            }
        }
        if (numModelos > filasVisibles) {  // barra de desplazamiento
            float altoPista = filasVisibles * 44.0f - 4, altoBarra = max(24.0f, altoPista * filasVisibles / numModelos);
            float yBarra = zonaLista.y + (altoPista - altoBarra) * modeloOff_ / (float)(numModelos - filasVisibles);
            DrawRectangleRounded(R(x0 + anchoDer - 6, zonaLista.y, 4, altoPista), 1.0f, 4, Al(C_LINEA, 0.8f));
            DrawRectangleRounded(R(x0 + anchoDer - 6, yBarra, 4, altoBarra), 1.0f, 4, C_PRIM);
            Texto("Rueda del mouse: " + to_string(numModelos) + " modelos", x0, ventana.y + 412, 11, C_PRIM);
        }
        size_t modeloActual = (size_t)Clamp((float)modeloSel_, 0, (float)p->modelos().size() - 1);
        TextoDer(Ajustar("Elegido: " + p->modelos()[modeloActual].nombre, 12, anchoDer * 0.6f), x0 + anchoDer, ventana.y + 412, 12, C_TINTA);

        // ---- Cantidad y botón Agregar ----
        int libres = p->stock() - t_.carrito().cantidadDe(p);  // unidades que aún se pueden agregar
        cantSel_ = max(1, min(cantSel_, max(1, libres)));
        Texto("CANTIDAD", x0, ventana.y + 436, 11, C_MUTED);
        Rectangle selector = R(x0, ventana.y + 456, 130, 46);
        Caja(selector, 0.5f, Al(C_SUAVE, 0.9f), C_LINEA);
        Rectangle botonMenos = R(selector.x, selector.y, 44, 46), botonMas = R(selector.x + 86, selector.y, 44, 46);
        bool encimaMenos = sobre(botonMenos), encimaMas = sobre(botonMas);
        DrawLineEx(Vector2{botonMenos.x + 15, botonMenos.y + 23}, Vector2{botonMenos.x + 29, botonMenos.y + 23}, 2,
                   encimaMenos ? C_PELIGRO : C_MUTED);
        DrawLineEx(Vector2{botonMas.x + 15, botonMas.y + 23}, Vector2{botonMas.x + 29, botonMas.y + 23}, 2, encimaMas ? C_TINTA : C_MUTED);
        DrawLineEx(Vector2{botonMas.x + 22, botonMas.y + 16}, Vector2{botonMas.x + 22, botonMas.y + 30}, 2, encimaMas ? C_TINTA : C_MUTED);
        TextoCentro(to_string(cantSel_), R(selector.x + 44, selector.y, 42, 46), 18, C_TINTA);
        if (encimaMenos) {
            manito_ = true;
            if (click_)
                luego([this] {
                    cantSel_ = max(1, cantSel_ - 1);
                });
        }
        if (encimaMas) {
            manito_ = true;
            if (click_)
                luego([this, libres] {
                    cantSel_ = min(max(1, libres), cantSel_ + 1);
                });
        }
        string textoAgregar = libres > 0 ? "Agregar · " + dinero(p->precioFinal(modeloSel_) * cantSel_)
                                         : string(p->disponible() ? "Ya tienes todo el stock" : "Agotado");
        if (boton(90, R(x0 + 148, ventana.y + 456, anchoDer - 148, 46), textoAgregar, C_ACENTO, libres > 0, 16))
            luego([this, p] {
                int agregadas = t_.carrito().agregar(p, modeloSel_, cantSel_);
                bump_ = 1;
                aviso(agregadas ? "Agregado · " + p->nombre() + " " + p->modelos()[(size_t)modeloSel_].nombre : "No hay más stock");
                ir(0);
            });
    }

    // Mensaje emergente en la parte superior
    void dibujarAviso() {
        if (toastA_ < 0.01f)
            return;
        float ancho = Ancho(toastMsg_, 15) + 80;
        Rectangle r = R(W / 2 - ancho / 2, -60 + toastA_ * 84, ancho, 46);
        Vidrio(r, 0.5f, 1.6f, C_MODAL);
        DrawCircleV(Vector2{r.x + 28, r.y + 23}, 13, C_OK);
        DrawLineEx(Vector2{r.x + 22, r.y + 23}, Vector2{r.x + 26, r.y + 27}, 2.5f, WHITE);
        DrawLineEx(Vector2{r.x + 26, r.y + 27}, Vector2{r.x + 34, r.y + 18}, 2.5f, WHITE);
        Texto(toastMsg_, r.x + 52, r.y + 14, 15, C_TINTA);
    }

    // Fondo casi blanco con manchas pastel suaves que se ven a través del vidrio
    void fondo() {
        float t = (float)GetTime();
        DrawRectangleGradientV(0, 0, (int)W + 1, (int)H + 1, Color{250, 251, 254, 255}, Color{240, 244, 252, 255});
        CirculoDeg(Vector2{W * 0.16f + sinf(t * 0.20f) * 80, H * 0.30f + cosf(t * 0.17f) * 50}, 400, Color{255, 214, 150, 80},
                   Color{255, 214, 150, 0});
        CirculoDeg(Vector2{W * 0.82f + cosf(t * 0.15f) * 80, H * 0.22f + sinf(t * 0.22f) * 50}, 440, Color{201, 220, 250, 150},
                   Color{201, 220, 250, 0});
        CirculoDeg(Vector2{W * 0.55f + sinf(t * 0.18f) * 100, H * 0.88f + cosf(t * 0.14f) * 40}, 420, Color{201, 220, 250, 110},
                   Color{201, 220, 250, 0});
    }

    // =============== ciclo de cada cuadro ===============

    // Lee teclado/mouse y actualiza animaciones y estado
    void leerEntradaYActualizar() {
        if (IsKeyPressed(KEY_F11))
            ToggleFullscreen();

        int anchoVentana = GetScreenWidth(), altoVentana = GetScreenHeight();
        SC = Clamp(min(anchoVentana / 1280.0f, altoVentana / 760.0f), 0.55f, 1.6f);
        W = anchoVentana / SC;
        H = altoVentana / SC;  // la interfaz se escala para llenar la ventana

        dt_ = GetFrameTime();
        mouse_ = Vector2{GetMouseX() / SC, GetMouseY() / SC};
        click_ = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        wheel_ = GetMouseWheelMove();
        enter_ = IsKeyPressed(KEY_ENTER) && capa_ == 2;
        manito_ = ibeam_ = clicCampo_ = false;

        // Animación de las capas, del carrito y del aviso
        for (int i = 1; i < 4; i++)
            capaA_[i] = Lerp(capaA_[i], capa_ == i ? 1.0f : 0.0f, Clamp(dt_ * 11, 0, 1));
        bump_ = Lerp(bump_, 0, Clamp(dt_ * 8, 0, 1));
        if (toast_ > 0)
            toast_ -= dt_;
        toastA_ = Lerp(toastA_, toast_ > 0 ? 1.0f : 0.0f, Clamp(dt_ * 8, 0, 1));

        // Teclado: escribir, borrar, Tab, Esc
        for (int caracter = GetCharPressed(); caracter > 0; caracter = GetCharPressed())
            if (foco_)
                foco_->teclear(caracter);
        if (foco_ && (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)))
            foco_->borrar();
        if (capa_ == 2 && IsKeyPressed(KEY_TAB)) {
            auto campos = camposPaso();
            if (!campos.empty()) {
                auto it = find(campos.begin(), campos.end(), foco_);
                foco_ = campos[it == campos.end() ? 0 : (size_t)(it - campos.begin() + 1) % campos.size()];
            }
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (capa_)
                ir(0);
            else {
                busca_.texto.clear();
                foco_ = nullptr;
            }
        }

        // Si cambia la búsqueda, vuelve arriba y salta a "Productos" si estabas en otra pestaña
        if (busca_.texto != prevBusca_) {
            prevBusca_ = busca_.texto;
            scrollObj_ = 0;
            if (!busca_.texto.empty() && (tab_ == 0 || tab_ == 4 || tab_ == 5)) {
                tab_ = 1;
                cat_ = -1;
            }
        }

        // Scroll de la página (suavizado)
        if (capa_ == 0 && wheel_ != 0 && !sobreBarra_)
            scrollObj_ -= wheel_ * 80;
        scrollObj_ = Clamp(scrollObj_, 0.0f, max(0.0f, contentH_ - (H - TOP)));
        scroll_ = Lerp(scroll_, scrollObj_, Clamp(dt_ * 12, 0, 1));

        // Banner principal: cambia solo cada 7 segundos
        heroFade_ = Clamp(heroFade_ + dt_ * 3, 0, 1);
        heroT_ += dt_;
        if (heroT_ > 7.0f)
            cambiarHero((heroIdx_ + 1) % (int)destacados_.size());
    }

    // Dibuja todo, de atrás hacia adelante
    void dibujarTodo() {
        BeginDrawing();
        ClearBackground(C_BG);
        rlPushMatrix();
        rlScalef(SC, SC, 1);
        fondo();
        dibujarPagina();
        dibujarTopbar();
        dibujarCarrito();
        dibujarCheckout();
        dibujarProducto();
        dibujarAviso();
        rlPopMatrix();
        EndDrawing();
    }

    // Procesa clics, cambios de capa y acciones pendientes al terminar el cuadro
    void cerrarCuadro() {
        if (click_ && !clicCampo_)
            foco_ = nullptr;  // clic fuera de un campo = pierde el foco
        if (capaSig_ != capa_)
            foco_ = nullptr;
        capa_ = capaSig_;

        vector<function<void()>> pendientes;
        pendientes.swap(cola_);
        for (auto& accion : pendientes)
            accion();

        SetMouseCursor(ibeam_ ? MOUSE_CURSOR_IBEAM : manito_ ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
    }

    void paso() {
        leerEntradaYActualizar();
        dibujarTodo();
        cerrarCuadro();
    }

public:
    Interfaz() {
        marcas_ = t_.marcas();
        cRepo_.texto = "10";

        auto buscarPorNombre = [&](const char* nombre) -> const Producto* {
            for (const Producto* p : t_.productos())
                if (p->nombre() == nombre)
                    return p;
            return nullptr;
        };
        for (const char* nombre : {"GPU", "Laptop", "Mouse inalambrico", "Windows (OS)"})  // banner rotativo
            if (auto p = buscarPorNombre(nombre))
                destacados_.push_back(p);
        for (const char* nombre : {"Memoria RAM", "SSD", "Teclado", "Office", "Parlante", "CPU"})  // "Recomendados para ti"
            if (auto p = buscarPorNombre(nombre))
                recomendados_.push_back(p);
    }

    void ejecutar() {
        SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
        InitWindow(1280, 800, "TecnoStore - Tienda de Computadoras (POO)");

        // La ventana se ajusta al tamaño de tu pantalla
        int monitor = GetCurrentMonitor(), anchoMonitor = GetMonitorWidth(monitor), altoMonitor = GetMonitorHeight(monitor);
        int w = min(1280, (int)(anchoMonitor * 0.94f)), h = min(800, (int)(altoMonitor * 0.88f));
        SetWindowSize(w, h);
        SetWindowPosition(max(0, (anchoMonitor - w) / 2), max(20, (altoMonitor - h) / 2 - 20));
        SetWindowMinSize(800, 520);
        SetExitKey(KEY_NULL);  // ESC no cierra el programa: cierra ventanas / limpia la búsqueda
        SetTargetFPS(60);
        cargarFuente();

        while (!WindowShouldClose())
            paso();

        for (auto& par : TEXTURAS)
            if (par.second.id)
                UnloadTexture(par.second);
        CloseWindow();
    }
};

// =====================================================================================
// 9. MAIN
// =====================================================================================
int main() {
    // 1. PRIMERO LA VENTANA (¡Siempre!)
    InitWindow(800, 450, "Mi Proyecto POO");

    // 2. DESPUÉS puedes cargar fuentes, texturas o imágenes tranquilamente
    Font miFuente = LoadFont("tu_fuente.ttf"); 

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Tu bucle del juego...
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}