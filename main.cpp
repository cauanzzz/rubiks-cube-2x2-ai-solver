#include <raylib.h>
#include <raymath.h>
#include <chrono>
#include <vector>
#include <string> 
#include <array>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <memory>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include "cubo.hpp"
#include "solver.hpp"
#include "render.hpp"

namespace jogo {
constexpr int LARGURA = 1280, ALTURA = 800, PAINEL = 840;
constexpr int LIMITE_IDDFS = 11;
constexpr Color FUNDO{13, 17, 30, 255}, CAIXA{24, 32, 51, 255};
constexpr Color BORDA{58, 77, 101, 255}, TEXTO{224, 234, 244, 255};
constexpr Color CIANO{78, 235, 209, 255}, AMARELO{255, 208, 92, 255};
enum class Tela { Menu, Solo, IA };
const char* nome(int i) { return i == 0 ? "BFS" : i == 1 ? "IDDFS" : "A*"; }
std::string decimal(double n) {
    std::ostringstream s; s << std::fixed << std::setprecision(2) << n; return s.str();
}
void texto(const std::string& s, int x, int y, int tam = 18, Color cor = TEXTO) {
    DrawText(s.c_str(), x, y, tam, cor);
}
bool botao(Rectangle r, const std::string& s, bool habilitado = true) {
    bool sobre = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec({r.x + 4, r.y + 4, r.width, r.height}, BLACK);
    DrawRectangleRec(r, habilitado && sobre ? BORDA : CAIXA);
    DrawRectangleLinesEx(r, 2, habilitado ? CIANO : BORDA);
    DrawText(s.c_str(), static_cast<int>(r.x + 10), static_cast<int>(r.y + (r.height - 18)/2),
             18, habilitado ? TEXTO : GRAY);
    return habilitado && sobre && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
bool aplicar(cubo& c, const std::string& passo) {
    for (const auto& m : obter_movimentos()) {
        if (m.nome == passo) { (c.*m.funcao)(); return true; }
    }
    return false;
}

std::string embaralhar(cubo& c, std::uint32_t seed, int quantidade) {
    c = cubo{};
    std::mt19937 gerador(seed);
    const auto movimentos = obter_movimentos();
    int anterior = -1;
    std::string sequencia;
    for (int i = 0; i < quantidade; ++i) {
        int indice;
        do { indice = static_cast<int>(gerador() % movimentos.size()); }
        while (anterior >= 0 && indice / 2 == anterior / 2);
        (c.*movimentos[indice].funcao)();
        if (!sequencia.empty()) sequencia += ' ';
        sequencia += movimentos[indice].nome;
        anterior = indice;
    }
    return sequencia;
}
struct Medicao {
    bool pronta = false;
    double ms = 0;
    ResultadoIA resultado;
    std::string erro;
};
struct Trabalho {
    std::mutex mutex;
    std::array<Medicao, 3> linhas{};
    std::atomic<int> executando{-1};
    std::atomic<bool> pronto{false};
};

void executar(std::shared_ptr<Trabalho> trabalho, cubo entrada, int escolhido) {
    for (int i = 0; i < 3; ++i) {
        if (escolhido != -1 && escolhido != i) continue;
        trabalho->executando.store(i);
        Medicao m;
        const auto inicio = std::chrono::steady_clock::now();
        try {
            if (i == 0) m.resultado = resolver_bfs(entrada);
            else if (i == 1) m.resultado = resolver_profundidade(entrada, LIMITE_IDDFS);
            else m.resultado = resolver_astar(entrada);
        } catch (const std::exception&) {
            m.erro = "Falha na busca (recursos/excecao).";
        } catch (...) {
            m.erro = "Falha inesperada na busca.";
        }
        m.ms = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - inicio).count();
        m.pronta = true;
        std::cout << "\n[" << nome(i) << "] ";

        if (!m.erro.empty()) {
            std::cout << m.erro;
        } else if (!m.resultado.encontrado) {
            std::cout << "Solucao nao encontrada";
        } else {
            std::cout << "Solucao (" << m.resultado.passos.size()
                    << " giros): ";

            if (m.resultado.passos.empty())
                std::cout << "Cubo ja resolvido";

            for (const auto& movimento : m.resultado.passos)
                std::cout << movimento << ' ';
        }

        std::cout << std::endl;
        { std::lock_guard<std::mutex> lock(trabalho->mutex);
          trabalho->linhas[i] = std::move(m); }
    }
    trabalho->executando.store(-1);
    trabalho->pronto.store(true);
}
struct App {
    Tela tela = Tela::Menu;
    cubo estado, origemBusca;
    std::string seed = "2026", sequencia = "Cubo inicial resolvido";
    bool editandoSeed = false, orbita = false, reproduzindo = false;
    int quantidade = 3, escolhido = -1, movimentosManuais = 0;
    float intervalo = 0.45f;
    double ultimoGiro = 0, inicioBusca = 0;
    std::size_t passo = 0;
    std::vector<std::string> solucao;
    std::string ultimo = "-", aviso;
    std::array<Medicao, 3> linhas{};
    std::shared_ptr<Trabalho> trabalho;
    float yaw = 0.75f, pitch = 0.55f;
    bool ocupado() const { return static_cast<bool>(trabalho); }
    void limparResultados() {
        linhas = {}; solucao.clear(); passo = 0; reproduzindo = false;
        ultimo = "-"; aviso.clear();
    }
    void reproduzir(int indice) {
        const auto& m = linhas[indice];
        if (!m.pronta || !m.erro.empty() || !m.resultado.encontrado) return;
        estado = origemBusca;
        solucao = m.resultado.passos;
        passo = 0; ultimo = "-";
        reproduzindo = !solucao.empty(); ultimoGiro = GetTime();
        aviso = solucao.empty() ? "O estado consultado ja estava resolvido." : "Reproduzindo solucao...";
    }
    void iniciar(int algoritmo) {
        if (ocupado()) return;
        limparResultados(); editandoSeed = false;
        origemBusca = estado; escolhido = algoritmo; inicioBusca = GetTime();
        auto novo = std::make_shared<Trabalho>();
        // nao acessa App nem Raylib. Encerrar o processo encerra a busca.
        try {
            std::thread worker(executar, novo, origemBusca, algoritmo);
            worker.detach(); trabalho = std::move(novo);
        } catch (const std::exception&) { aviso = "Nao foi possivel iniciar a thread."; }
    }
    void atualizarBusca() {
        if (!trabalho) return;
        const bool terminou = trabalho->pronto.load();
        { std::lock_guard<std::mutex> lock(trabalho->mutex);
          for (int i = 0; i < 3; ++i)
              if (!linhas[i].pronta && trabalho->linhas[i].pronta)
                  linhas[i] = trabalho->linhas[i]; }
        if (!terminou) return;
        trabalho.reset();
        aviso = escolhido < 0 ? "Comparacao concluida: mesma entrada nos 3 metodos." : "Busca concluida.";
        if (escolhido >= 0) {
            auto& m = linhas[escolhido];
            if (!m.erro.empty()) aviso = m.erro;
            else if (!m.resultado.encontrado)
                aviso = escolhido == 1 ? "Sem solucao ate o limite 11." : "Solucao nao encontrada.";
            else reproduzir(escolhido);
        }
    }
    void avancar() {
        if (passo >= solucao.size()) { reproduzindo = false; return; }
        ultimo = solucao[passo];
        if (!aplicar(estado, ultimo)) {
            aviso = "Movimento desconhecido no resultado."; reproduzindo = false; return;
        }
        ++passo; ultimoGiro = GetTime();
        if (passo == solucao.size()) {
            reproduzindo = false;
            aviso = estado.verificador() ? "Solucao aplicada!" : "Resultado nao resolveu o cubo.";
        }
    }
};
void fundo() {
    ClearBackground(FUNDO);
    for (int x = 0; x < LARGURA; x += 32) DrawLine(x, 0, x, ALTURA, Color{20, 26, 42, 255});
    for (int y = 0; y < ALTURA; y += 32) DrawLine(0, y, LARGURA, y, Color{20, 26, 42, 255});
}
void menu(App& a) {
    texto("CUBE / 2x2", 425, 130, 64, CIANO);
    texto("PUZZLE LAB  //  PLAYER + SEARCH AI", 417, 212, 20, AMARELO);
    const Color cores[] = {WHITE, YELLOW, ORANGE, GREEN, RED, BLUE};
    for (int i = 0; i < 6; ++i) {
        DrawRectangle(486 + i*52, 276, 40, 40, cores[i]);
        DrawRectangleLines(486 + i*52, 276, 40, 40, BLACK);
    }
    if (botao({420, 375, 440, 58}, "1 - MODO SOLO (JOGADOR)") || IsKeyPressed(KEY_ONE)) a.tela = Tela::Solo;
    if (botao({420, 457, 440, 58}, "2 - MODO ASSISTIDO POR IA") || IsKeyPressed(KEY_TWO)) a.tela = Tela::IA;
    texto("F R U L D B  /  SHIFT: GIRO INVERSO", 430, 578, 18);
    texto("MOUSE DIREITO + ARRASTAR: CAMERA", 441, 610, 18);
    texto("ESC: sair", 581, 724, 18, GRAY);
    if (IsKeyPressed(KEY_ESCAPE)) a.aviso = "sair";
}
void cameraECubo(App& a) {
    Rectangle area{0, 70, static_cast<float>(PAINEL), 615};
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && CheckCollisionPointRec(GetMousePosition(), area)) a.orbita = true;
    if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) a.orbita = false;
    if (a.orbita) {
        Vector2 d = GetMouseDelta(); a.yaw -= d.x*0.008f;
        a.pitch = std::clamp(a.pitch + d.y*0.008f, -1.45f, 1.45f);
    }
    Camera3D camera{};
    const float raio = 7.0f;
    camera.position = {raio*std::cos(a.pitch)*std::sin(a.yaw), raio*std::sin(a.pitch), raio*std::cos(a.pitch)*std::cos(a.yaw)};
    camera.target = {0, 0, 0}; camera.up = {0, 1, 0};
    camera.fovy = 40; camera.projection = CAMERA_PERSPECTIVE;
    BeginMode3D(camera);
    desenharCubo3D(a.estado);
    EndMode3D();
}
void painel(App& a) {
    DrawRectangle(PAINEL, 70, LARGURA - PAINEL, ALTURA - 70, CAIXA);
    texto("CONFIGURACAO", 860, 90, 22, CIANO);
    const bool livre = !a.ocupado() && !a.reproduzindo;
    Rectangle campo{860, 126, 245, 36};
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        a.editandoSeed = livre && CheckCollisionPointRec(GetMousePosition(), campo);
    DrawRectangleRec(campo, FUNDO);
    DrawRectangleLinesEx(campo, 2, a.editandoSeed ? AMARELO : BORDA);
    texto("Seed: " + a.seed + (a.editandoSeed ? "_" : ""), 870, 135, 18);
    int ch;
    while ((ch = GetCharPressed()) > 0) {
        if (a.editandoSeed && ch >= '0' && ch <= '9' && a.seed.size() < 10) a.seed += static_cast<char>(ch);
    }
    if (a.editandoSeed && IsKeyPressed(KEY_BACKSPACE) && !a.seed.empty()) a.seed.pop_back();
    if (IsKeyPressed(KEY_ENTER)) a.editandoSeed = false;
    texto("Giros: " + std::to_string(a.quantidade), 860, 185);
    if (botao({1010, 176, 42, 32}, "-", livre)) a.quantidade = std::max(1, a.quantidade - 1);
    if (botao({1062, 176, 42, 32}, "+", livre)) a.quantidade = std::min(15, a.quantidade + 1);
    if (botao({860, 222, 244, 36}, "EMBARALHAR COM SEED", livre)) {
        std::uint64_t valor = 0;
        for (char c : a.seed) valor = valor*10 + static_cast<unsigned>(c - '0');
        if (a.seed.empty() || valor > UINT32_MAX) a.aviso = "Seed deve estar entre 0 e 4294967295.";
        else {
            a.limparResultados(); a.movimentosManuais = 0;
            a.sequencia = embaralhar(a.estado, static_cast<std::uint32_t>(valor), a.quantidade);
        }
    }
    if (botao({1118, 222, 142, 36}, "RESET", livre)) {
        a.estado = cubo{}; a.limparResultados(); a.movimentosManuais = 0; a.sequencia = "Cubo inicial resolvido";
    }
    texto("Mesma seed + giros = mesmo embaralhamento", 860, 272, 15, GRAY);
    if (a.tela == Tela::Solo) {
        texto("CONTROLES", 860, 332, 22, CIANO);
        texto("F R U L D B: giro horario", 860, 379);
        texto("SHIFT + tecla: anti-horario", 860, 410);
        texto("Mouse direito: orbitar", 860, 451);
        texto("Movimentos: " + std::to_string(a.movimentosManuais), 860, 506, 22, AMARELO);
        return;
    }
    texto("RESOLVER ESTADO ATUAL", 860, 316, 20, CIANO);
    for (int i = 0; i < 3; ++i)
        if (botao({860.0f + i*137, 350, 124, 38}, nome(i), livre)) a.iniciar(i);
    if (botao({860, 401, 398, 38}, "COMPARAR ALGORITMOS", livre)) a.iniciar(-1);
    texto("Algoritmo    ms         Nos       Giros", 860, 460, 16, AMARELO);
    for (int i = 0; i < 3; ++i) {
        int y = 493 + i*37; const auto& m = a.linhas[i];
        texto(nome(i), 860, y, 16);
        if (!m.pronta) { texto("--", 965, y, 16, GRAY); continue; }
        texto(decimal(m.ms), 949, y, 14);
        texto(std::to_string(m.resultado.estadosVisitados), 1060, y, 14);
        texto(!m.erro.empty() ? "erro" : !m.resultado.encontrado ? "n/a" : std::to_string(m.resultado.passos.size()), 1185, y, 14);
    }
    texto("Nos: contador original de cada solver", 860, 607, 15, GRAY);
    texto("IDDFS: limite 11 / n/a = nao encontrado", 860, 629, 15, GRAY);
    texto("REPRODUZIR RESULTADO", 860, 668, 18, CIANO);
    for (int i = 0; i < 3; ++i) {
        const auto& m = a.linhas[i];
        if (botao({860.0f + i*137, 700, 124, 35}, nome(i), !a.ocupado() && m.pronta && m.erro.empty() && m.resultado.encontrado)) a.reproduzir(i);
    }
    texto("Comece com 3-4 giros para comparar.", 860, 762, 16, AMARELO);
}
} 

int main() {
    using namespace jogo;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(LARGURA, ALTURA, "CUBE / 2x2 - Search AI Lab");
    if (!IsWindowReady()) return 1;
    SetExitKey(KEY_NULL); SetTargetFPS(60);
    RenderTexture2D vista = LoadRenderTexture(PAINEL, 615);
    if (vista.id == 0) { CloseWindow(); return 1; }
    SetTextureFilter(vista.texture, TEXTURE_FILTER_POINT);
    App a;
    while (!WindowShouldClose()) {
        a.atualizarBusca();
        if (a.tela != Tela::Menu && !a.ocupado() && !a.reproduzindo && !a.editandoSeed) {
            const int teclas[] = {KEY_F, KEY_R, KEY_U, KEY_L, KEY_D, KEY_B};
            const auto movimentos = obter_movimentos();
            bool inverso = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
            for (int i = 0; i < 6; ++i) if (IsKeyPressed(teclas[i])) {
                a.limparResultados(); (a.estado.*movimentos[i*2 + (inverso ? 1 : 0)].funcao)();
                ++a.movimentosManuais;
            }
        }
        if (a.reproduzindo && GetTime() - a.ultimoGiro >= a.intervalo) a.avancar();
        if (a.tela != Tela::Menu) {
            BeginTextureMode(vista); ClearBackground(FUNDO); cameraECubo(a); EndTextureMode();
        }
        BeginDrawing(); fundo();
        if (a.tela == Tela::Menu) menu(a);
        else {
            DrawTextureRec(vista.texture, {0, 0, static_cast<float>(PAINEL), -615}, {0, 70}, WHITE);
            texto(a.tela == Tela::Solo ? "CUBE / SOLO" : "CUBE / AI LAB", 25, 22, 30, CIANO);
            if (botao({650, 15, 170, 38}, "MENU / ESC", !a.ocupado()) || (IsKeyPressed(KEY_ESCAPE) && !a.ocupado())) {
                a.tela = Tela::Menu; a.reproduzindo = false; a.editandoSeed = false;
            }
            painel(a);
            texto("F R U L D B + SHIFT  |  Arraste com mouse direito", 25, 87, 17, GRAY);
            if (a.estado.verificador()) texto("CUBO RESOLVIDO!", 260, 142, 32, AMARELO);
            texto("SEQUENCIA: " + a.sequencia, 25, 595, 16, CIANO);
            if (a.tela == Tela::IA && !a.solucao.empty()) {
            std::string solucaoTexto = "SOLUCAO: ";

            for (const auto& movimento : a.solucao) {
                solucaoTexto += movimento + " ";
            }

            texto(solucaoTexto, 25, 617, 16, AMARELO);
}
            if (a.ocupado()) {
                int i = a.trabalho->executando.load();
                texto(std::string("BUSCANDO ") + (i >= 0 ? nome(i) : "...") + "  " + decimal((GetTime()-a.inicioBusca)*1000) + " ms", 25, 632, 22, AMARELO);
                texto("Camera liberada. Aguarde o solver terminar.", 25, 664, 18);
            } else {
                texto(a.aviso, 25, 642, 17, AMARELO);
                if (a.tela == Tela::IA) {
                    texto("Passo " + std::to_string(a.passo) + "/" + std::to_string(a.solucao.size()) + "  Ultimo: " + a.ultimo, 25, 680, 18);
                    if (botao({25, 717, 135, 36}, a.reproduzindo ? "PAUSAR" : "CONTINUAR", a.passo < a.solucao.size())) {
                        a.reproduzindo = !a.reproduzindo; a.ultimoGiro = GetTime();
                    }
                    if (botao({173, 717, 120, 36}, "1 PASSO", !a.reproduzindo && a.passo < a.solucao.size())) a.avancar();
                    texto("Intervalo: " + decimal(a.intervalo) + " s", 330, 728, 18);
                    if (botao({566, 717, 42, 36}, "-")) a.intervalo = std::max(0.1f, a.intervalo - 0.1f);
                    if (botao({622, 717, 42, 36}, "+")) a.intervalo = std::min(2.0f, a.intervalo + 0.1f);
                }
            }
        }
        EndDrawing();
        if (a.tela == Tela::Menu && a.aviso == "sair") break;
    }
    UnloadRenderTexture(vista); CloseWindow();
    return 0;
}
