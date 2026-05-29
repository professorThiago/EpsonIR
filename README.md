# EpsonIR

Biblioteca Arduino/ESP32 para **enviar comandos IR para projetores Epson** usando o protocolo NEC Extended obtido por engenharia reversa.

> Desenvolvida e testada com um ESP32-S3 e um controle remoto Epson real.  
> Autor: [professorThiago](https://github.com/professorThiago)

---

## Sobre o protocolo

O controle Epson envia **dois frames NEC** a cada botão pressionado:

| Frame | Valor | Função |
|-------|-------|--------|
| Frame 1 (wake-up) | `0x81C00FF0` | Idêntico para todos os botões |
| Frame 2 (comando) | `0xC1AA<~cmd><cmd>` | Identifica o botão pressionado |

Layout dos bytes do Frame 2 (LSB primeiro, conforme reportado pela IRremoteESP8266):

```
B3    B2    B1      B0
0xC1  0xAA  <~cmd>  <cmd>
fixo  fixo  chksum  código do botão
```

`B0 + B1 == 0xFF` sempre — verificação de integridade padrão NEC.

---

## Instalação

### PlatformIO (recomendado)

Adicione ao seu `platformio.ini`:

```ini
lib_deps =
    https://github.com/professorThiago/EpsonIR
    crankyoldgit/IRremoteESP8266 @ ^2.9.0
```

### Arduino IDE

1. Baixe este repositório como arquivo ZIP.
2. **Sketch → Incluir Biblioteca → Adicionar biblioteca .ZIP…**
3. Instale também a **IRremoteESP8266** pelo Gerenciador de Bibliotecas.

---

## Ligação de hardware

### Transmissão (LED IR / módulo emissor)

```
ESP32 GPIO 4  →  TX/IN do módulo emissor IR
3,3 V ou 5 V  →  VCC do módulo emissor
GND           →  GND do módulo emissor
```

> Um LED IR simples precisa de um resistor de ~100 Ω em série e um transistor NPN (ex: 2N2222) como driver. Módulos emissores prontos já incluem esses componentes.

### Recepção (para o exemplo MapButtons)

```
ESP32 GPIO 15  →  OUT do receptor IR  (VS1838B / TSOP4838 / TSOP31238)
3,3 V          →  VCC do receptor
GND            →  GND do receptor
```

---

## Quick start

```cpp
#include <EpsonIR.h>

EpsonIR projetor(4);   // LED IR no GPIO 4

void setup() {
    projetor.begin();
}

void loop() {
    projetor.send(EPSON_CMD_POWER);   // liga / desliga
    delay(5000);
}
```

---

## API

### Construtor

```cpp
EpsonIR projetor(uint8_t pinoTX);
```

### Métodos

#### `void begin()`
Inicializa o emissor IR. Chame uma vez no `setup()`.

#### `void send(uint8_t commandCode)`
Envia o comando completo em dois frames para o projetor.

```cpp
projetor.send(EPSON_CMD_FREEZE);    // congela imagem
projetor.send(EPSON_CMD_VOL_UP);    // volume +
projetor.send(EPSON_CMD_HDMI);      // entrada HDMI
```

#### `bool sendRawFrame(uint32_t frame2)`
Envia um comando usando o valor de 32 bits bruto do Frame 2 (por exemplo, capturado com o exemplo MapButtons). Retorna `false` se o frame não passar na validação do protocolo.

```cpp
projetor.sendRawFrame(0xC1AA09F6UL);   // Power
```

#### `static uint32_t buildFrame(uint8_t commandCode)`
Retorna o valor de 32 bits do Frame 2 para um código de comando, sem transmitir. Útil para log ou reenvio posterior.

```cpp
uint32_t frame = EpsonIR::buildFrame(EPSON_CMD_POWER);
// frame == 0xC1AA09F6
```

#### `static bool isValidFrame(uint32_t frame2)`
Retorna `true` se o valor passa na validação do protocolo Epson (B2==0xAA, B3==0xC1, B0+B1==0xFF).

---

## Referência de comandos

| Constante | Código | Botão |
|-----------|--------|-------|
| `EPSON_CMD_POWER` | `0x09` | Liga / desliga |
| `EPSON_CMD_FREEZE` | `0x49` | Congela a imagem |
| `EPSON_CMD_MUTE` | `0xC9` | Silencia o áudio |
| `EPSON_CMD_ESC` | `0x21` | Voltar / cancelar |
| `EPSON_CMD_ENTER` | `0xA1` | Confirmar / enter |
| `EPSON_CMD_UP` | `0x0D` | Navegar para cima |
| `EPSON_CMD_DOWN` | `0x4D` | Navegar para baixo |
| `EPSON_CMD_RIGHT` | `0x8D` | Navegar para direita |
| `EPSON_CMD_LEFT` | `0xCD` | Navegar para esquerda |
| `EPSON_CMD_HOME` | `0xA9` | Tela inicial |
| `EPSON_CMD_MENU` | `0x59` | Abrir / fechar menu |
| `EPSON_CMD_VOL_UP` | `0x19` | Volume + |
| `EPSON_CMD_VOL_DOWN` | `0x99` | Volume - |
| `EPSON_CMD_ZOOM_IN` | `0x11` | Zoom + |
| `EPSON_CMD_ZOOM_OUT` | `0x91` | Zoom - |
| `EPSON_CMD_HDMI` | `0xCE` | Entrada HDMI |
| `EPSON_CMD_COMPUTER` | `0x29` | Entrada Computer |
| `EPSON_CMD_USB` | `0x6E` | Entrada USB |
| `EPSON_CMD_LAN` | `0x2E` | Entrada LAN |
| `EPSON_CMD_SOURCE_SEARCH` | `0x31` | Busca automática de fonte |
| `EPSON_CMD_COLOR_MODE` | `0xF1` | Modo de cor |
| `EPSON_CMD_ASPECT` | `0x51` | Proporção de tela |
| `EPSON_CMD_SPLIT` | `0x4B` | Dividir tela |
| `EPSON_CMD_0` … `EPSON_CMD_9` | `0x79` … | Teclas numéricas |
| `EPSON_CMD_ID` | `0x8C` | ID do projetor |
| `EPSON_CMD_USER` | `0xF9` | Botão de usuário |
| `EPSON_CMD_DEFAULT` | `0x9C` | Restaurar padrões |

---

## Exemplos

| Exemplo | Descrição |
|---------|-----------|
| `SendCommand` | Envia o comando POWER a cada 5 segundos |
| `MapButtons` | Recebe sinais IR e imprime o nome do botão decodificado |

---

## Licença

MIT © 2026 [professorThiago](https://github.com/professorThiago)
