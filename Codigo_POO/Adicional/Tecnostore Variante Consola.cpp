// =====================================================================================
// TIENDA DE COMPUTADORAS "TecnoStore" - C++17 (POO) - VERSIÓN CONSOLA
// =====================================================================================
// Compilar:  g++ -std=c++17 -O2 tecnostore_consola.cpp -o tecnostore
//
// Cupones: TECNO10 (10%) · BIENVENIDO (S/ 20, desde S/ 100) · GAMER50 (S/ 50, desde S/ 1000)
//          PRIMERA5 (5%) · ACEITUNO (90%)
//
// Conceptos de POO usados:
//   Abstracción     : Producto, Persona, MedioPago, Comprobante, Envio, Promocion, Cupon
//   Encapsulamiento : atributos protected/private + getters
//   Herencia        : Producto > Componente > CPU...   |   Persona > Trabajador > Vendedor...
//   Polimorfismo    : precioFinal, detalle, recargo, validar, emitir, comision, calcular
//
// INDICE
//   1. UTILIDADES   2. PERSONAS   3. PRODUCTOS   4. PAGO, COMPROBANTE, ENVÍO, PROMOCIONES, CUPONES, CARRITO
//   5. TIENDA       6. INTERFAZ DE CONSOLA        7. MAIN

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

// =====================================================================================
// 1. UTILIDADES
// =====================================================================================

static string dec(double valor, int decimales = 2) {
    long long factor = 1;
    for (int i = 0; i < decimales; i++)
        factor *= 10;

    bool negativo = valor < 0;
    if (negativo)
        valor = -valor;

    long long total = (long long)(valor * factor + 0.5);
    if (total == 0)
        negativo = false;

    string resultado = to_string(total / factor);
    if (decimales > 0) {
        string fraccion = to_string(total % factor);
        while ((int)fraccion.size() < decimales)
            fraccion = "0" + fraccion;
        resultado += "." + fraccion;
    }
    return (negativo ? "-" : "") + resultado;
}

static string ceros(int numero, int ancho) {
    string texto = to_string(numero);
    while ((int)texto.size() < ancho)
        texto = "0" + texto;
    return texto;
}

static int limitar(int valor, int minimo, int maximo) {
    return valor < minimo ? minimo : valor > maximo ? maximo : valor;
}

static string dinero(double valor) {
    return "S/ " + dec(valor);
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

static vector<string> dividir(const string& texto, char separador = '|') {
    vector<string> partes;
    stringstream flujo(texto);
    string parte;
    while (getline(flujo, parte, separador))
        partes.push_back(parte);
    return partes;
}

// =====================================================================================
// 2. PERSONAS
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

    virtual string rol() const = 0;
};

class Cliente : public Persona {
    string tipo_;
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

    virtual double comision(double) const {
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
        return "Vendedor · comisión " + dec(porcentajeComision_, 1) + "%";
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
// 3. PRODUCTOS
// =====================================================================================

struct Modelo {
    string nombre;
    double factor;
    bool recomendado;
};

static vector<Modelo> parseModelos(const string& texto) {
    vector<Modelo> modelos;
    for (string item : dividir(texto)) {
        bool recomendado = !item.empty() && item.back() == '*';
        if (recomendado)
            item.pop_back();
        size_t posDosPuntos = item.rfind(':');
        if (posDosPuntos == string::npos)
            continue;
        modelos.push_back({item.substr(0, posDosPuntos), atof(item.substr(posDosPuntos + 1).c_str()), recomendado});
    }
    return modelos;
}

struct Ficha {
    string codigo, nombre, marca;
    double precio;
    int garantiaMeses, stock;
    string modelos = "Básico:0.8|Estándar:1*|Pro:1.6";
};

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
    for (const auto& entrada : tabla) {
        const string& clave = entrada.first;
        const string& modelos = entrada.second;
        bool exacta = clave[0] == '=';
        bool coincide = exacta ? marcaMinuscula == clave.substr(1) : marcaMinuscula.find(clave) != string::npos;
        if (coincide)
            return dividir(modelos);
    }
    return dividir(
        "Modelo estándar|Generación actual|Generación anterior|Versión Pro|Edición básica|Edición avanzada|Edición gamer|Edición profesional");
}

static string textoModelosElegidos(const vector<string>& elegidos) {
    string resultado;
    for (size_t i = 0; i < elegidos.size(); i++)
        resultado += (resultado.empty() ? "" : "|") + elegidos[i] + ":1" + (i == 0 ? "*" : "");
    return resultado.empty() ? "Estándar:1*" : resultado;
}

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

enum Categoria { HARDWARE = 0, ACCESORIOS, PERIFERICOS, SOFTWARE, EQUIPOS, SERVICIOS };

class Producto {
protected:
    string codigo_, nombre_, marca_;
    double precioBase_;
    int categoria_, garantiaMeses_, stock_;
    vector<Modelo> modelos_;
    bool oferta_ = false;
    mutable double desde_ = -1;

public:
    Producto(const Ficha& ficha, int categoria)
        : codigo_(ficha.codigo), nombre_(ficha.nombre), marca_(ficha.marca), precioBase_(ficha.precio),
          categoria_(categoria), garantiaMeses_(ficha.garantiaMeses), stock_(ficha.stock),
          modelos_(parseModelos(ficha.modelos)) {
        if (ficha.precio < 0)
            throw invalid_argument("Precio invalido");
    }

    virtual ~Producto() = default;

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

    void fijarModelos(const string& texto) {
        auto nuevos = parseModelos(texto);
        if (!nuevos.empty()) {
            modelos_ = nuevos;
            desde_ = -1;
        }
    }

    string modelosTexto() const {
        string resultado;
        for (const auto& m : modelos_)
            resultado += (resultado.empty() ? "" : "|") + m.nombre + ":" + dec(m.factor, 4) + (m.recomendado ? "*" : "");
        return resultado;
    }

    int recomendado() const {
        for (size_t i = 0; i < modelos_.size(); i++)
            if (modelos_[i].recomendado)
                return (int)i;
        return 0;
    }

    double precioModelo(int indice) const {
        if (modelos_.empty())
            return precioBase_;
        size_t i = (size_t)limitar(indice, 0, (int)modelos_.size() - 1);
        return precioBase_ * modelos_[i].factor;
    }

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

    void marcarOferta() {
        oferta_ = true;
    }

    virtual bool enOferta() const {
        return oferta_;
    }

    virtual double precioFinal(int modelo = 0) const {
        return precioModelo(modelo);
    }

    double desdePrecio() const {
        if (desde_ < 0) {
            desde_ = precioFinal(0);
            for (size_t i = 1; i < modelos_.size(); i++)
                desde_ = min(desde_, precioFinal((int)i));
        }
        return desde_;
    }

    virtual string detalle() const = 0;
};

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
        return to_string(nucleos_) + " núcleos · " + to_string(hilos_) + " hilos · " + dec(ghz_, 1) + " GHz · " +
               to_string(potenciaW_) + " W";
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
        return to_string(vramGB_) + " GB VRAM · " + dec(ghz_, 1) + " GHz · " + to_string(nucleos_) + " núcleos";
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
        return to_string(capacidadGB_) + " GB " + tipoDDR_ + " · " + to_string(mhz_) + " MHz";
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
        return formato_ + " · " + to_string(capacidadGB_) + " GB · " + to_string(lecturaMBs_) + "/" +
               to_string(escrituraMBs_) + " MB/s";
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
        return "Socket " + socket_ + " · " + chipset_ + " · " + to_string(ranurasRAM_) + " ranuras RAM";
    }
};

class FuentePoder : public Componente {
    string certificacion_;

public:
    FuentePoder(const Ficha& ficha, int potenciaW, string certificacion)
        : Componente(ficha, potenciaW), certificacion_(move(certificacion)) {
    }

    string detalle() const override {
        return to_string(potenciaW_) + " W · " + certificacion_;
    }
};

class Ventilador : public Componente {
    int rpm_, diametroMm_;

public:
    Ventilador(const Ficha& ficha, int potenciaW, int rpm, int diametroMm)
        : Componente(ficha, potenciaW), rpm_(rpm), diametroMm_(diametroMm) {
    }

    string detalle() const override {
        return to_string(rpm_) + " RPM · " + to_string(diametroMm_) + " mm";
    }
};

class Accesorio : public Producto {
protected:
    string compatibilidad_;

public:
    Accesorio(const Ficha& ficha, string compatibilidad)
        : Producto(ficha, ACCESORIOS), compatibilidad_(move(compatibilidad)) {
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
        return tipo_ + " " + to_string(capacidadGB_) + " GB · " + compatibilidad_;
    }
};

class PastaTermica : public Accesorio {
    int gramos_;

public:
    PastaTermica(const Ficha& ficha, string compatibilidad, int gramos)
        : Accesorio(ficha, move(compatibilidad)), gramos_(gramos) {
    }

    string detalle() const override {
        return to_string(gramos_) + " gramos · " + compatibilidad_;
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
        return "Conector " + conector_ + " · " + dec(metros_, 1) + " m";
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
        return conector_ + " · " + to_string(potenciaW_) + " W · " + compatibilidad_;
    }
};

class Periferico : public Producto {
    string conexion_;
    bool inalambrico_, bluetooth_;

protected:
    string conectividad() const {
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
        return conectividad() + " · " + to_string(dpi_) + " DPI" + (rgb_ ? " · RGB" : "");
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
        return conectividad() + " · " + to_string(potenciaW_) + " W";
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
        return resolucion_ + " · " + to_string(fps_) + " fps · " + conectividad();
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

class Software : public Producto {
    bool permanente_;
    int meses_;

protected:
    string licencia() const {
        return permanente_ ? string("Licencia permanente") : "Licencia de " + to_string(meses_) + " meses";
    }

public:
    Software(const Ficha& ficha, bool permanente, int meses)
        : Producto(ficha, SOFTWARE), permanente_(permanente), meses_(meses) {
    }

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

class EquipoCompleto : public Producto {
    string procesador_, ram_, almacenamiento_;

protected:
    string especificaciones() const {
        return procesador_ + " · " + ram_ + " · " + almacenamiento_;
    }

public:
    EquipoCompleto(const Ficha& ficha, string procesador, string ram, string almacenamiento)
        : Producto(ficha, EQUIPOS), procesador_(move(procesador)), ram_(move(ram)),
          almacenamiento_(move(almacenamiento)) {
    }

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
    Laptop(const Ficha& ficha, string procesador, string ram, string almacenamiento, double pulgadas, int bateriaWh,
           bool oled)
        : EquipoCompleto(ficha, move(procesador), move(ram), move(almacenamiento)), pulgadas_(pulgadas),
          bateriaWh_(bateriaWh), oled_(oled) {
    }

    string detalle() const override {
        return dec(pulgadas_, 1) + "\" " + (oled_ ? "OLED" : "LED") + " · " + to_string(bateriaWh_) + " Wh · " +
               especificaciones();
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
        return tecnologia_ + " · " + to_string(paginasPorMinuto_) + " ppm" + (tieneEscaner_ ? " · Escáner" : "");
    }
};

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
        return descripcion_ + " · vigencia " + to_string(vigenciaMeses_) + " meses";
    }
};

// =====================================================================================
// 4. PAGO, COMPROBANTE, ENVÍO, PROMOCIONES, CUPONES, CARRITO
// =====================================================================================

class MedioPago {
public:
    virtual ~MedioPago() = default;
    virtual string nombre() const = 0;
    virtual string campo() const = 0;
    virtual string validar(const string& dato, double total) const = 0;
    virtual string resumen(const string& dato, double total) const = 0;

    virtual double recargo(double) const {
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
        bool ok = dato.size() == 4 && all_of(dato.begin(), dato.end(), ::isdigit);
        return ok ? "" : "Escribe los 4 últimos dígitos";
    }

    string resumen(const string& dato, double) const override {
        return "Tarjeta terminada en " + dato;
    }

    double recargo(double monto) const override {
        return monto * 0.03;
    }
};

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
        bool valido = dato.size() == 9 && dato[0] == '9' && all_of(dato.begin(), dato.end(), ::isdigit);
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

struct Linea {
    const Producto* producto;
    int modelo;
    int cantidad;
};

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

class PromoVolumen : public Promocion {
    int minimoProductos_;
    double porcentaje_;

public:
    PromoVolumen(int minimoProductos, double porcentaje) : minimoProductos_(minimoProductos), porcentaje_(porcentaje) {
    }

    string nombre() const override {
        return "Volumen: " + to_string(minimoProductos_) + "+ productos";
    }

    double descuento(const vector<Linea>& lineas) const override {
        int unidades = 0;
        for (auto& l : lineas)
            unidades += l.cantidad;
        return unidades >= minimoProductos_ ? subtotalDe(lineas) * porcentaje_ : 0;
    }
};

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

class Carrito {
    vector<Linea> lineas_;
    vector<unique_ptr<Promocion>> promociones_;

public:
    void agregarPromo(unique_ptr<Promocion> promo) {
        promociones_.push_back(move(promo));
    }

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

    int cantidad() const {
        int total = 0;
        for (auto& l : lineas_)
            total += l.cantidad;
        return total;
    }

    int cantidadDe(const Producto* producto) const {
        int total = 0;
        for (auto& l : lineas_)
            if (l.producto == producto)
                total += l.cantidad;
        return total;
    }

    double subtotal() const {
        return subtotalDe(lineas_);
    }

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

struct Pedido {
    string numero, fecha, cliente, documento, entrega, vendedor, pago, comprobante, cupon;
    double total = 0, comision = 0;
    vector<string> items;
};

// =====================================================================================
// 5. TIENDA
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

    static constexpr const char* ARCHIVO = "inventario.dat";

    template <class T, class... Args> void agregarAlCatalogo(Args&&... args) {
        catalogo_.push_back(make_unique<T>(forward<Args>(args)...));
    }

    template <class T> static vector<string> nombresDe(const vector<unique_ptr<T>>& lista) {
        vector<string> resultado;
        for (auto& elemento : lista)
            resultado.push_back(elemento->nombre());
        return resultado;
    }

    static string limpio(string texto) {
        for (char& c : texto)
            if (c == '|' || c == '\t' || c == '\n' || c == '\r')
                c = ' ';
        return texto;
    }

    void guardarInventario() const {
        ofstream archivo(ARCHIVO, ios::trunc);
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
        ifstream archivo(ARCHIVO);
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
                        Ficha ficha{codigo, nombre, marca, atof(precio.c_str()), atoi(garantia.c_str()),
                                    atoi(stock.c_str()), modelos};
                        catalogo_.push_back(make_unique<ProductoPersonalizado>(
                            ficha, limitar(atoi(categoria.c_str()), 0, 5), detalle));
                    }
                }
            } catch (...) {
            }
        }
    }

    void crearCatalogoBase() {
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

        agregarAlCatalogo<MemoriaFlash>(Ficha{"USB-01", "Memoria USB", "Kingston", 35, 12, 40, "32 GB:0.7|64 GB:1*|128 GB:1.8"}, "Universal", 64, "USB");
        agregarAlCatalogo<MemoriaFlash>(Ficha{"SD-01", "Memoria SD", "SanDisk", 40, 12, 30, "32 GB:0.7|64 GB:1*|128 GB:1.8"}, "Cámaras y celulares", 64, "SD");
        agregarAlCatalogo<PastaTermica>(Ficha{"TIM-01", "Pasta termica", "Arctic", 15, 6, 50, "4 g:0.8|8 g:1*|30 g:2.5"}, "CPU y GPU", 8);
        agregarAlCatalogo<Cable>(Ficha{"CAB-01", "Cable de poder", "Genérico", 20, 6, 35, "1.8 m:1*|3 m:1.5"}, "Fuentes ATX", "C13", 1.8);
        agregarAlCatalogo<Adaptador>(Ficha{"ADP-01", "HDMI a VGA", "Ugreen", 30, 12, 22, "Estándar:1*|Con audio:1.4"}, "Monitores y proyectores", "HDMI", "VGA");
        agregarAlCatalogo<Cargador>(Ficha{"CHG-01", "Cargador tipo C", "Anker", 55, 12, 18, "20 W:0.7|45 W:1*|65 W:1.5"}, "Celulares y laptops", "USB-C", 45);
        agregarAlCatalogo<Cable>(Ficha{"SATA-01", "Extension SATA", "Genérico", 12, 3, 40, "0.5 m:1*|1 m:1.4"}, "Discos SATA", "SATA", 0.5);

        agregarAlCatalogo<Mouse>(Ficha{"MOU-01", "Mouse inalambrico", "Logitech", 50, 12, 25, "Básico:0.8|Ergonómico:1*|Gamer RGB:1.8"}, "USB 2.4 GHz", true, false, 1600, false);
        agregarAlCatalogo<Mouse>(Ficha{"MOU-02", "Mouse con cable", "Redragon", 25, 12, 30, "Básico:0.8|Estándar:1*|Gamer:1.7"}, "USB", false, false, 3200, true);
        agregarAlCatalogo<Teclado>(Ficha{"TEC-01", "Teclado", "Redragon", 70, 12, 18, "Membrana:0.8|Mecánico:1.4*|Mecánico RGB:2"}, "USB", false, false, "Mecánico", true);
        agregarAlCatalogo<Parlante>(Ficha{"PAR-01", "Parlante", "JBL", 90, 12, 12, "Mini:0.7|Estándar:1*|Potente:1.8"}, "Bluetooth", true, true, 20);
        agregarAlCatalogo<CamaraWeb>(Ficha{"CAM-01", "Camara web", "Logitech", 120, 12, 10, "720p:0.7|1080p:1*|4K:2"}, "USB", false, false, 30, "1080p");
        agregarAlCatalogo<Microfono>(Ficha{"MIC-01", "Microfono", "HyperX", 110, 12, 9, "Estándar:0.8|Condensador:1*|Pro:1.8"}, "USB", false, false, "Condensador");
        agregarAlCatalogo<AdaptadorInalambrico>(Ficha{"WIFI-01", "Adaptador WiFi", "TP-Link", 40, 12, 20, "WiFi 5:0.8|WiFi 6:1*"}, "USB", true, true, "WiFi + Bluetooth 5.0");

        agregarAlCatalogo<SistemaOperativo>(Ficha{"OS-01", "Windows (OS)", "Microsoft", 450, 0, 99, "Home:0.7|Pro:1*|Enterprise:1.6"}, true, 0, "64 bits");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-01", "Adobe", "Adobe", 300, 0, 99, "1 mes:0.2|12 meses:1*|24 meses:1.8"}, false, 12, "Diseño y edición");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-02", "Avast", "Avast", 90, 0, 99, "1 PC:0.6|3 PC:1*|5 PC:1.4"}, false, 12, "Antivirus");
        agregarAlCatalogo<Aplicacion>(Ficha{"APP-03", "Office", "Microsoft", 380, 0, 99, "Hogar y Estudiante:0.8|Hogar y Empresas:1*|Profesional:1.5"}, true, 0, "Ofimática");

        agregarAlCatalogo<Computadora>(Ficha{"PC-01", "Computadora", "HP", 2500, 12, 5, "Oficina:0.7|Hogar:1*|Gamer:1.9"}, "Core i5", "16 GB", "512 GB SSD", "ATX");
        agregarAlCatalogo<Laptop>(Ficha{"LAP-01", "Laptop", "Lenovo", 3200, 12, 6, "14 pulg Básica:0.7|15.6 pulg Estándar:1*|16 pulg Pro:1.7"}, "Ryzen 5", "16 GB", "512 GB SSD", 15.6, 60, false);
        agregarAlCatalogo<Impresora>(Ficha{"PRN-01", "Impresora", "Epson", 600, 6, 8, "Inyección de tinta:0.7|Multifuncional:1*|Láser:1.6"}, "Tinta continua", 15, true);

        agregarAlCatalogo<Mantenimiento>(Ficha{"SRV-01", "Mantenimiento", "TecnoStore", 80, 0, 99, "Básico:0.7|Completo:1*|Premium:1.6"}, "Limpieza y optimización", 3);
        agregarAlCatalogo<Garantia>(Ficha{"SRV-02", "Garantia", "TecnoStore", 150, 0, 99, "6 meses:0.6|12 meses:1*|24 meses:1.8"}, "Extensión de garantía", "gratuito");
        agregarAlCatalogo<Servicio>(Ficha{"SRV-03", "Armado de PC", "TecnoStore", 100, 0, 99, "Estándar:1*|Con pruebas:1.4"}, "Armado, cableado y pruebas");

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
        cargarInventario();
    }

    size_t cantidadProductos() const {
        return catalogo_.size();
    }

    vector<const Producto*> productos() const {
        vector<const Producto*> resultado;
        for (auto& p : catalogo_)
            resultado.push_back(p.get());
        return resultado;
    }

    string siguienteCodigo() const {
        for (int n = 1; n < 10000; n++) {
            string codigo = "PRD-" + ceros(n, 3);
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
        catalogo_.push_back(make_unique<ProductoPersonalizado>(ficha, limitar(categoria, 0, 5), move(detalle)));
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

    vector<string> marcas() const {
        vector<string> resultado;
        for (auto& p : catalogo_)
            if (find(resultado.begin(), resultado.end(), p->marca()) == resultado.end())
                resultado.push_back(p->marca());
        sort(resultado.begin(), resultado.end(), [](const string& a, const string& b) { return minusc(a) < minusc(b); });
        return resultado;
    }

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

    vector<string> nombresMedios() const {
        return nombresDe(mediosPago_);
    }

    vector<string> nombresComprobantes() const {
        return nombresDe(comprobantes_);
    }

    vector<string> nombresEnvios() const {
        return nombresDe(envios_);
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

    double descuentoCupon() const {
        if (!cuponAplicado_ || carrito_.cantidad() == 0)
            return 0;
        return cuponAplicado_->descuento(carrito_.subtotal() - carrito_.descuentos());
    }

    double costoEnvio() const {
        return carrito_.cantidad() ? envios_[envioElegido_]->costo() : 0;
    }

    double base() const {
        return carrito_.subtotal() - carrito_.descuentos() - descuentoCupon() + costoEnvio();
    }

    double recargo() const {
        return carrito_.cantidad() ? mediosPago_[medioElegido_]->recargo(base()) : 0;
    }

    double total() const {
        return base() + recargo();
    }

    Pedido pagar(const Cliente& cliente, const string& ruc, const string& razonSocial, const string& direccion,
                 const string& datoPago, int indiceVendedor) {
        Pedido pedido;
        pedido.total = total();
        pedido.cupon = cuponAplicado_ ? cuponAplicado_->codigo() : string();

        const Comprobante& comprobante = *comprobantes_[comprobanteElegido_];
        const Envio& envio = *envios_[envioElegido_];

        time_t ahora = time(nullptr);
        tm* fecha = localtime(&ahora);
        pedido.numero = comprobante.serie() + "-" + ceros(++contadorPedidos_, 6);
        pedido.fecha = ceros(fecha->tm_mday, 2) + "/" + ceros(fecha->tm_mon + 1, 2) + "/" + to_string(fecha->tm_year + 1900);
        pedido.cliente = cliente.nombreCompleto();
        pedido.documento = comprobante.pideRUC() ? "RUC " + ruc + " · " + razonSocial : "DNI " + cliente.datos().dni;
        pedido.entrega = envio.nombre() + (envio.pideDireccion() ? " · " + direccion : string());

        const Vendedor* vendedor = vendedores()[(size_t)indiceVendedor];
        pedido.vendedor = vendedor->nombreCompleto();
        pedido.comision = vendedor->comision(pedido.total);
        pedido.comprobante = comprobante.emitir(pedido.total);
        pedido.pago = mediosPago_[medioElegido_]->resumen(datoPago, pedido.total);

        for (auto& linea : carrito_.lineas()) {
            pedido.items.push_back(to_string(linea.cantidad) + "× " + linea.producto->nombre() + " " +
                                   linea.producto->modelos()[(size_t)linea.modelo].nombre);
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
// 6. INTERFAZ DE CONSOLA
// =====================================================================================

namespace Consola {

static const char* CATEGORIAS[6] = {"Hardware", "Accesorios", "Periféricos", "Software", "Equipos", "Servicios"};

struct CompraCancelada {};

static size_t largoVisible(const string& s) {
    size_t n = 0;
    for (unsigned char c : s)
        if ((c & 0xC0) != 0x80)
            n++;
    return n;
}

// Rellena con espacios (cuenta caracteres UTF-8, no bytes) y recorta si no cabe.
static string rellenar(string s, size_t ancho) {
    if (largoVisible(s) > ancho) {
        while (largoVisible(s) > ancho - 1) {
            s.pop_back();
            while (!s.empty() && ((unsigned char)s.back() & 0xC0) == 0x80)
                s.pop_back();
        }
        s += "…";
    }
    return s + string(ancho - largoVisible(s), ' ');
}

static string recortar(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos)
        return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static string leer(const string& mensaje) {
    cout << mensaje << flush;
    string linea;
    if (!getline(cin, linea)) {
        cout << "\nHasta pronto.\n";
        exit(0);
    }
    return recortar(linea);
}

static bool soloDigitos(const string& s) {
    return !s.empty() && all_of(s.begin(), s.end(), [](unsigned char c) { return isdigit(c); });
}

// Devuelve un entero o -1 si el texto no es un número válido.
static int aEntero(const string& s) {
    return soloDigitos(s) && s.size() < 9 ? atoi(s.c_str()) : -1;
}

static void titulo(const string& texto) {
    string linea(64, '=');
    cout << "\n" << linea << "\n  " << texto << "\n" << linea << "\n";
}

static void subtitulo(const string& texto) {
    cout << "\n--- " << texto << " " << string(max<int>(0, 58 - (int)largoVisible(texto)), '-') << "\n";
}

static void pausa() {
    leer("\nPresiona Enter para continuar...");
}

static bool confirmar(const string& pregunta) {
    while (true) {
        string r = minusc(leer(pregunta + " (s/n): "));
        if (r == "s" || r == "si" || r == "sí")
            return true;
        if (r == "n" || r == "no")
            return false;
    }
}

class Aplicacion {
    Tienda t_;

    struct Filtro {
        int categoria = -1;
        string marca, busqueda;
        int orden = 0;
    };

    // ------------------------------------------------------------------ utilidades UI
    int elegir(const string& encabezado, const vector<string>& opciones, int porDefecto = 0) {
        cout << "\n" << encabezado << "\n";
        for (size_t i = 0; i < opciones.size(); i++)
            cout << "  " << (i + 1) << ") " << opciones[i] << "\n";
        while (true) {
            string r = leer("Elige una opción [" + to_string(porDefecto + 1) + "]: ");
            if (r.empty())
                return porDefecto;
            int n = aEntero(r);
            if (n >= 1 && n <= (int)opciones.size())
                return n - 1;
            cout << "  ! Opción no válida\n";
        }
    }

    // Pide un dato; repite hasta que valida() devuelva "". Escribir "cancelar" aborta la compra.
    string pedir(const string& etiqueta, function<string(const string&)> valida) {
        while (true) {
            string s = leer(etiqueta + ": ");
            if (minusc(s) == "cancelar")
                throw CompraCancelada{};
            string error = valida(s);
            if (error.empty())
                return s;
            cout << "  ! " << error << "\n";
        }
    }

    static string minimo(const string& s, size_t n, const char* msg) {
        return s.size() >= n ? "" : msg;
    }

    // ------------------------------------------------------------------ filtros / listado
    vector<const Producto*> filtrar(const Filtro& f, int modo) const {
        vector<const Producto*> resultado;
        string busqueda = minusc(f.busqueda);
        for (const Producto* p : t_.productos()) {
            if (modo == 1 && !p->enOferta())
                continue;
            if (modo == 2 && p->categoria() != SERVICIOS)
                continue;
            if (modo == 0 && f.categoria >= 0 && p->categoria() != f.categoria)
                continue;
            if (!f.marca.empty() && p->marca() != f.marca)
                continue;
            if (!busqueda.empty() &&
                minusc(p->nombre() + " " + p->marca() + " " + p->detalle()).find(busqueda) == string::npos)
                continue;
            resultado.push_back(p);
        }
        if (f.orden)
            stable_sort(resultado.begin(), resultado.end(), [&](const Producto* a, const Producto* b) {
                return f.orden == 1 ? a->desdePrecio() < b->desdePrecio() : a->desdePrecio() > b->desdePrecio();
            });
        return resultado;
    }

    void tablaProductos(const vector<const Producto*>& lista) const {
        cout << "\n" << rellenar("N°", 4) << rellenar("Código", 9) << rellenar("Producto", 22) << rellenar("Marca", 14)
             << rellenar("Categoría", 13) << rellenar("Desde", 12) << "Estado\n";
        cout << string(88, '-') << "\n";
        for (size_t i = 0; i < lista.size(); i++) {
            const Producto* p = lista[i];
            cout << rellenar(to_string(i + 1), 4) << rellenar(p->codigo(), 9) << rellenar(p->nombre(), 22)
                 << rellenar(p->marca(), 14) << rellenar(CATEGORIAS[p->categoria()], 13)
                 << rellenar(dinero(p->desdePrecio()), 12) << p->estado() << (p->enOferta() ? "  [OFERTA]" : "")
                 << "\n";
        }
        if (lista.empty())
            cout << "  (sin resultados · prueba otra palabra o quita el filtro)\n";
    }

    // ------------------------------------------------------------------ catálogo
    void catalogo(int modo) {
        Filtro f;
        const char* nombres[3] = {"CATÁLOGO DE PRODUCTOS", "OFERTAS", "SERVICIOS"};
        while (true) {
            auto lista = filtrar(f, modo);
            titulo(nombres[modo]);
            string estado = to_string(lista.size()) + " resultados";
            if (modo == 0 && f.categoria >= 0)
                estado += " · categoría: " + string(CATEGORIAS[f.categoria]);
            if (!f.marca.empty())
                estado += " · marca: " + f.marca;
            if (!f.busqueda.empty())
                estado += " · búsqueda: \"" + f.busqueda + "\"";
            const char* ord[3] = {"original", "menor precio", "mayor precio"};
            estado += " · orden: " + string(ord[f.orden]);
            cout << estado << "\n";
            tablaProductos(lista);

            cout << "\n[N°] ver producto   [C] categoría   [M] marca   [B] buscar   [O] ordenar   [L] limpiar   [K] carrito   [0] volver\n";
            string r = minusc(leer("> "));
            if (r == "0")
                return;
            int n = aEntero(r);
            if (n >= 1 && n <= (int)lista.size()) {
                verProducto(lista[(size_t)n - 1]);
            } else if (r == "c" && modo == 0) {
                vector<string> op = {"Todas"};
                for (auto* c : CATEGORIAS)
                    op.push_back(c);
                f.categoria = elegir("Categoría:", op, f.categoria + 1) - 1;
            } else if (r == "m") {
                auto marcas = t_.marcas();
                vector<string> op = {"Todas"};
                op.insert(op.end(), marcas.begin(), marcas.end());
                int i = elegir("Marca:", op, 0);
                f.marca = i == 0 ? "" : marcas[(size_t)i - 1];
            } else if (r == "b") {
                f.busqueda = leer("Buscar producto o marca (Enter = quitar): ");
            } else if (r == "o") {
                f.orden = elegir("Ordenar por:", {"Original", "Menor precio", "Mayor precio"}, f.orden);
            } else if (r == "l") {
                f = Filtro{};
            } else if (r == "k") {
                carrito();
            } else {
                cout << "  ! Opción no válida\n";
            }
        }
    }

    void verProducto(const Producto* p) {
        titulo(p->nombre() + " · " + p->marca());
        cout << CATEGORIAS[p->categoria()] << " · código " << p->codigo() << "\n";
        cout << p->detalle() << "\n";
        cout << (p->garantia() > 0 ? "Garantía " + to_string(p->garantia()) + " meses" : string("Sin garantía extendida"))
             << " · " << p->estado() << " (" << p->stock() << " en stock)" << (p->enOferta() ? " · OFERTA" : "")
             << "\n";

        subtitulo("Elige tu modelo");
        for (size_t i = 0; i < p->modelos().size(); i++) {
            const Modelo& m = p->modelos()[i];
            cout << "  " << rellenar(to_string(i + 1) + ")", 5) << rellenar(m.nombre, 32)
                 << rellenar(dinero(p->precioFinal((int)i)), 12) << (m.recomendado ? "RECOMENDADO" : "") << "\n";
        }

        int libres = p->stock() - t_.carrito().cantidadDe(p);
        if (libres <= 0) {
            cout << "\n" << (p->disponible() ? "Ya tienes todo el stock en tu carrito." : "Producto agotado.") << "\n";
            pausa();
            return;
        }

        int modelo = -1;
        while (modelo < 0) {
            string r = leer("\nNúmero de modelo [Enter = recomendado, 0 = volver]: ");
            if (r == "0")
                return;
            if (r.empty()) {
                modelo = p->recomendado();
                break;
            }
            int n = aEntero(r);
            if (n >= 1 && n <= (int)p->modelos().size())
                modelo = n - 1;
            else
                cout << "  ! Modelo no válido\n";
        }

        int cantidad = 0;
        while (cantidad <= 0) {
            string r = leer("Cantidad (máx. " + to_string(libres) + ") [1]: ");
            int n = r.empty() ? 1 : aEntero(r);
            if (n >= 1 && n <= libres)
                cantidad = n;
            else
                cout << "  ! Cantidad no válida\n";
        }

        int agregadas = t_.carrito().agregar(p, modelo, cantidad);
        if (agregadas)
            cout << "\n✔ Agregado: " << agregadas << "× " << p->nombre() << " " << p->modelos()[(size_t)modelo].nombre
                 << " · " << dinero(p->precioFinal(modelo) * agregadas) << "\n";
        else
            cout << "\nNo hay más stock.\n";
        pausa();
    }

    // ------------------------------------------------------------------ carrito
    void mostrarCarrito() {
        Carrito& c = t_.carrito();
        if (c.lineas().empty()) {
            cout << "  Tu carrito está vacío.\n";
            return;
        }
        cout << "\n" << rellenar("N°", 4) << rellenar("Producto", 34) << rellenar("Cant.", 7) << rellenar("P. unit.", 12)
             << "Importe\n"
             << string(70, '-') << "\n";
        for (size_t i = 0; i < c.lineas().size(); i++) {
            const Linea& l = c.lineas()[i];
            cout << rellenar(to_string(i + 1), 4)
                 << rellenar(l.producto->nombre() + " " + l.producto->modelos()[(size_t)l.modelo].nombre, 34)
                 << rellenar(to_string(l.cantidad), 7) << rellenar(dinero(l.producto->precioFinal(l.modelo)), 12)
                 << dinero(l.producto->precioFinal(l.modelo) * l.cantidad) << "\n";
        }
        cout << string(70, '-') << "\n";
        cout << rellenar("Subtotal", 57) << dinero(c.subtotal()) << "\n";
        for (auto& d : c.detalleDescuentos())
            cout << rellenar("  " + d.first, 57) << "-" << dinero(d.second) << "\n";
        cout << rellenar("Total parcial", 57) << dinero(c.subtotal() - c.descuentos()) << "\n";
        cout << "(El envío, el cupón y el pago se eligen al finalizar la compra)\n";
    }

    void carrito() {
        while (true) {
            titulo("MI CARRITO");
            mostrarCarrito();
            Carrito& c = t_.carrito();
            if (c.lineas().empty()) {
                pausa();
                return;
            }
            cout << "\n[1] Finalizar compra   [2] Quitar una unidad   [3] Vaciar carrito   [0] Volver\n";
            string r = leer("> ");
            if (r == "0")
                return;
            if (r == "1") {
                if (checkout())
                    return;
            } else if (r == "2") {
                int n = aEntero(leer("N° de línea: "));
                if (n >= 1 && n <= (int)c.lineas().size())
                    c.quitar((size_t)n - 1);
                else
                    cout << "  ! Línea no válida\n";
            } else if (r == "3") {
                if (confirmar("¿Vaciar todo el carrito?"))
                    c.vaciar();
            } else {
                cout << "  ! Opción no válida\n";
            }
        }
    }

    // ------------------------------------------------------------------ checkout
    bool checkout() {
        if (t_.carrito().cantidad() == 0)
            return false;
        try {
            titulo("FINALIZAR COMPRA  (escribe \"cancelar\" en cualquier dato para salir)");

            subtitulo("1. Tus datos");
            string nombres = pedir("Nombres", [](const string& s) { return minimo(s, 2, "Escribe tus nombres"); });
            string apP = pedir("Apellido paterno", [](const string& s) { return minimo(s, 2, "Escribe tu apellido paterno"); });
            string apM = pedir("Apellido materno", [](const string& s) { return minimo(s, 2, "Escribe tu apellido materno"); });
            string dni = pedir("DNI", [](const string& s) { return s.size() == 8 && soloDigitos(s) ? "" : "El DNI tiene 8 dígitos"; });
            string tel = pedir("Celular", [](const string& s) {
                return s.size() == 9 && soloDigitos(s) && s[0] == '9' ? "" : "9 dígitos, empieza con 9";
            });
            string mail = pedir("Correo electrónico", [](const string& s) {
                size_t a = s.find('@');
                return a != string::npos && a > 0 && s.find('.', a) != string::npos ? "" : "Correo no válido";
            });

            subtitulo("2. Comprobante y entrega");
            int comp = elegir("Comprobante:", t_.nombresComprobantes(), 0);
            t_.elegirComprobante(comp);
            string ruc, razon;
            if (t_.comprobanteActual().pideRUC()) {
                ruc = pedir("RUC (11 dígitos, empieza con 10 o 20)", [](const string& s) {
                    bool ok = s.size() == 11 && soloDigitos(s) && (s.rfind("10", 0) == 0 || s.rfind("20", 0) == 0);
                    return ok ? "" : "RUC de 11 dígitos (10 o 20)";
                });
                razon = pedir("Razón social", [](const string& s) { return minimo(s, 3, "Escribe la razón social"); });
            } else {
                cout << "La boleta se emite con el DNI que ingresaste.\n";
            }
            int env = elegir("Entrega:", t_.nombresEnvios(), 0);
            t_.elegirEnvio(env);
            string direccion;
            if (t_.envioActual().pideDireccion())
                direccion = pedir("Dirección de entrega", [](const string& s) { return minimo(s, 6, "Escribe tu dirección"); });
            else
                cout << "Recoge tu pedido en tienda · Lun a Sáb 9:00 - 20:00\n";

            subtitulo("3. Pago");
            vector<string> nombresVend;
            auto vendedores = t_.vendedores();
            for (auto* v : vendedores)
                nombresVend.push_back(v->nombreCompleto());
            int vend = elegir("Atendido por:", nombresVend, 0);
            int medio = elegir("Medio de pago:", t_.nombresMedios(), 0);
            t_.elegirMedio(medio);

            t_.quitarCupon();
            while (true) {
                string codigo = leer("\nCupón de descuento (Enter para omitir): ");
                if (minusc(codigo) == "cancelar")
                    throw CompraCancelada{};
                if (codigo.empty())
                    break;
                string error = t_.aplicarCupon(codigo);
                if (error.empty()) {
                    cout << "✔ Cupón aplicado: " << t_.cupon()->codigo() << " · " << t_.cupon()->descripcion() << "\n";
                    break;
                }
                cout << "  ! " << error << "\n";
            }

            subtitulo("Resumen");
            resumenCompra();

            string dato = pedir(t_.medioActual().campo(),
                                [&](const string& s) { return t_.medioActual().validar(s, t_.total()); });
            cout << "  " << t_.medioActual().resumen(dato, t_.total()) << "\n";

            if (!confirmar("\n¿Confirmar el pago de " + dinero(t_.total()) + "?")) {
                t_.quitarCupon();
                cout << "Compra no confirmada. Tu carrito sigue guardado.\n";
                pausa();
                return false;
            }

            Cliente cliente(DatosPersona{nombres, apP, apM, tel, mail, dni},
                            t_.comprobanteActual().pideRUC() ? "empresa" : "persona", direccion);
            Pedido pedido = t_.pagar(cliente, ruc, razon, direccion, dato, vend);
            comprobanteFinal(pedido, mail);
            pausa();
            return true;
        } catch (const CompraCancelada&) {
            t_.quitarCupon();
            cout << "\nCompra cancelada. Tu carrito sigue guardado.\n";
            pausa();
            return false;
        }
    }

    void resumenCompra() {
        Carrito& c = t_.carrito();
        mostrarCarrito();
        if (t_.descuentoCupon() > 0)
            cout << rellenar("  Cupón " + t_.cupon()->codigo(), 57) << "-" << dinero(t_.descuentoCupon()) << "\n";
        cout << rellenar("Envío", 57) << (t_.costoEnvio() > 0 ? dinero(t_.costoEnvio()) : string("Gratis")) << "\n";
        if (t_.recargo() > 0)
            cout << rellenar("Recargo tarjeta (3%)", 57) << dinero(t_.recargo()) << "\n";
        cout << rellenar("TOTAL A PAGAR", 57) << dinero(t_.total()) << "\n";
        (void)c;
    }

    void comprobanteFinal(const Pedido& p, const string& mail) {
        titulo("¡COMPRA REALIZADA!");
        cout << "Comprobante N° " << p.numero << " · " << p.fecha << "\n";
        cout << p.comprobante << "\n";
        cout << "Cliente : " << p.cliente << "\n";
        cout << "Documento: " << p.documento << "\n";
        cout << "Entrega : " << p.entrega << "\n";
        cout << "Atendió : " << p.vendedor << "\n";
        cout << "Pago    : " << p.pago << "\n";
        if (!p.cupon.empty())
            cout << "Cupón   : " << p.cupon << "\n";
        cout << "\nProductos:\n";
        for (auto& s : p.items)
            cout << "  - " << s << "\n";
        cout << "\nTOTAL PAGADO: " << dinero(p.total) << "\n";
        cout << "Enviaremos el detalle a " << mail << "\n";
    }

    // ------------------------------------------------------------------ pedidos / nosotros
    void pedidos() {
        titulo("MIS PEDIDOS");
        auto& lista = t_.pedidos();
        cout << lista.size() << " compras realizadas\n";
        if (lista.empty())
            cout << "\nAún no tienes pedidos · tus comprobantes aparecerán aquí.\n";
        for (int i = (int)lista.size() - 1; i >= 0; i--) {
            const Pedido& p = lista[(size_t)i];
            subtitulo("N° " + p.numero + " · " + p.fecha);
            cout << "Cliente : " << p.cliente << " · " << p.documento << "\n";
            for (auto& s : p.items)
                cout << "  - " << s << "\n";
            cout << "Entrega : " << p.entrega << "\n";
            cout << "Pago    : " << p.pago << "\n";
            if (!p.cupon.empty())
                cout << "Cupón   : " << p.cupon << "\n";
            cout << "Atendió : " << p.vendedor << " (comisión " << dinero(p.comision) << ")\n";
            cout << "TOTAL   : " << dinero(p.total) << "\n";
        }
        pausa();
    }

    void nosotros() {
        titulo("SOBRE TECNOSTORE");
        cout << "Tienda de computadoras: hardware, accesorios, periféricos, software,\n"
                "equipos completos y servicio técnico.\n"
                "Horario: Lun a Sáb 9:00 - 20:00 · Tel. 951 000 000 · ventas@tecnostore.pe\n"
                "Pagos: Efectivo, Tarjeta, Yape y Plin\n";
        subtitulo("Nuestro equipo");
        for (auto& t : t_.equipo())
            cout << "  " << rellenar(t->nombreCompleto(), 28) << rellenar(t->codigo(), 8) << t->rol() << "\n";
        subtitulo("Proveedores");
        for (auto& p : t_.proveedores())
            cout << "  " << rellenar(p->razon(), 28) << p->detalle() << " · " << p->rol() << "\n";
        pausa();
    }

    // ------------------------------------------------------------------ inventario
    void inventario() {
        while (true) {
            titulo("INVENTARIO");
            cout << "[1] Ver stock   [2] Agregar producto nuevo   [3] Reponer stock   [0] Volver\n";
            string r = leer("> ");
            if (r == "0")
                return;
            if (r == "1")
                verStock();
            else if (r == "2")
                agregarProducto();
            else if (r == "3")
                reponer();
            else
                cout << "  ! Opción no válida\n";
        }
    }

    void verStock() {
        string busqueda = minusc(leer("Filtrar por nombre/marca/código (Enter = todos): "));
        int total = 0, agotados = 0, mostrados = 0;
        cout << "\n" << rellenar("Código", 10) << rellenar("Producto", 24) << rellenar("Marca", 14) << rellenar("Categoría", 13)
             << "Stock / estado\n"
             << string(78, '-') << "\n";
        for (const Producto* p : t_.productos()) {
            total += p->stock();
            if (!p->disponible())
                agotados++;
            if (!busqueda.empty() &&
                minusc(p->nombre() + " " + p->marca() + " " + p->codigo()).find(busqueda) == string::npos)
                continue;
            mostrados++;
            cout << rellenar(p->codigo(), 10) << rellenar(p->nombre(), 24) << rellenar(p->marca(), 14)
                 << rellenar(CATEGORIAS[p->categoria()], 13) << p->stock() << " · " << p->estado() << "\n";
        }
        if (mostrados == 0)
            cout << "  No hay productos con ese nombre\n";
        cout << "\nTotal en tienda: " << total << " unidades · " << agotados << " agotados\n";
        pausa();
    }

    void agregarProducto() {
        subtitulo("Nuevo producto (deja el nombre vacío para cancelar)");
        string nombre = leer("Nombre del producto: ");
        if (nombre.empty())
            return;
        string marca = leer("Marca: ");
        if (marca.empty()) {
            cout << "  ! La marca es obligatoria\n";
            return;
        }
        string precioTxt = leer("Precio (S/): ");
        double precio = atof(precioTxt.c_str());
        string stockTxt = leer("Unidades iniciales: ");
        int stock = aEntero(stockTxt);
        if (precio <= 0 || stock < 0) {
            cout << "  ! Completa precio (mayor a 0) y stock inicial (número entero)\n";
            return;
        }

        vector<string> cats(CATEGORIAS, CATEGORIAS + 6);
        int categoria = elegir("Categoría:", cats, 0);

        // Modelos sugeridos según la marca
        vector<string> opciones = modelosDeMarca(marca);
        cout << "\nModelos de " << marca << " (elige uno o varios separados por coma, Enter = Estándar):\n";
        for (size_t i = 0; i < opciones.size(); i++)
            cout << "  " << (i + 1) << ") " << opciones[i] << "\n";
        vector<string> elegidos;
        for (const string& parte : dividir(leer("Modelos: "), ',')) {
            int n = aEntero(recortar(parte));
            if (n >= 1 && n <= (int)opciones.size() &&
                find(elegidos.begin(), elegidos.end(), opciones[(size_t)n - 1]) == elegidos.end())
                elegidos.push_back(opciones[(size_t)n - 1]);
        }

        string detalle = leer("Descripción breve (Enter = \"Producto nuevo\"): ");
        string codigo = t_.siguienteCodigo();
        Ficha ficha{codigo, nombre, marca, precio, 0, stock, textoModelosElegidos(elegidos)};
        if (t_.agregarPersonalizado(ficha, categoria, detalle.empty() ? "Producto nuevo" : detalle))
            cout << "\n✔ Producto agregado (" << codigo << ") con " << (elegidos.empty() ? 1 : elegidos.size())
                 << " opciones · " << nombre << "\n";
        else
            cout << "  ! No se pudo guardar el producto\n";
        pausa();
    }

    void reponer() {
        string codigo = mayusc(leer("Código del producto (ej. CPU-01): "));
        if (codigo.empty())
            return;
        const Producto* encontrado = nullptr;
        for (const Producto* p : t_.productos())
            if (p->codigo() == codigo)
                encontrado = p;
        if (!encontrado) {
            cout << "  ! No existe un producto con ese código\n";
            pausa();
            return;
        }
        cout << encontrado->nombre() << " · " << encontrado->marca() << " · stock actual: " << encontrado->stock() << "\n";
        string r = leer("Unidades a reponer [10]: ");
        int cantidad = r.empty() ? 10 : aEntero(r);
        if (cantidad > 0 && t_.reponerStock(codigo, cantidad))
            cout << "✔ Se agregaron " << cantidad << " unidades al stock\n";
        else
            cout << "  ! Cantidad no válida\n";
        pausa();
    }

public:
    void ejecutar() {
        while (true) {
            titulo("TECNOSTORE · Tienda de computadoras");
            cout << "  1) Productos\n"
                    "  2) Ofertas\n"
                    "  3) Servicios\n"
                    "  4) Mi carrito ("
                 << t_.carrito().cantidad() << " · " << dinero(t_.carrito().subtotal() - t_.carrito().descuentos())
                 << ")\n"
                    "  5) Mis pedidos\n"
                    "  6) Nosotros\n"
                    "  7) Inventario\n"
                    "  0) Salir\n";
            string r = leer("\n> ");
            if (r == "1")
                catalogo(0);
            else if (r == "2")
                catalogo(1);
            else if (r == "3")
                catalogo(2);
            else if (r == "4")
                carrito();
            else if (r == "5")
                pedidos();
            else if (r == "6")
                nosotros();
            else if (r == "7")
                inventario();
            else if (r == "0") {
                cout << "\n¡Gracias por visitar TecnoStore!\n";
                return;
            } else
                cout << "  ! Opción no válida\n";
        }
    }
};

} // namespace Consola

// =====================================================================================
// 7. MAIN
// =====================================================================================
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    Consola::Aplicacion app;
    app.ejecutar();
    return 0;
}
