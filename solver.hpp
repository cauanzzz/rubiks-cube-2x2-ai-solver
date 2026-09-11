#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include "cubo.hpp"

struct ResultadoIA 
{
    std::vector<std::string> passos;
    int estadosVisitados = 0;
    bool encontrado = false;
};

struct NoBusca 
{
    cubo estadoBusca;
    std::vector<std::string> historicomovimentos;
};

struct Movimento 
    { 
        std::string nome; 
        void (cubo::*funcao)(); 
    };

inline std::vector<Movimento> obter_movimentos() 
{
    return 
    {
        {"F",  &cubo::girar_f}, {"F'", &cubo::girar_antihorariof},
        {"R",  &cubo::girar_r}, {"R'", &cubo::girar_antihorarior},
        {"U",  &cubo::girar_u}, {"U'", &cubo::girar_antihorariou},
        {"L",  &cubo::girar_l}, {"L'", &cubo::girar_antihorariol},
        {"D",  &cubo::girar_d}, {"D'", &cubo::girar_antihorariod},
        {"B",  &cubo::girar_b}, {"B'", &cubo::girar_antihorariob}
    };
}

inline ResultadoIA resolver_bfs(cubo inicial) 
{
    ResultadoIA resultado;
    resultado.estadosVisitados = 0;

    std::queue<NoBusca> fila;
    std::unordered_map<std::string, bool> visitados;

    fila.push({inicial, {}});
    visitados[inicial.obter_chave()] = true;

    auto movimentos12 = obter_movimentos();

    while (!fila.empty()) 
    {
        NoBusca atual = fila.front();
        fila.pop();

        resultado.estadosVisitados++;

        if (atual.estadoBusca.verificador()) {
            resultado.passos = atual.historicomovimentos;
            resultado.encontrado = true;
            return resultado;
        }

        for (auto& m : movimentos12) {
            cubo proximo = atual.estadoBusca;
            (proximo.*(m.funcao))();

            std::string chave = proximo.obter_chave();
            if (!visitados[chave]) {
                visitados[chave] = true;
                
                std::vector<std::string> novoHistorico = atual.historicomovimentos;
                novoHistorico.push_back(m.nome);
                
                fila.push({proximo, novoHistorico});
            }
        }
    }

    resultado.encontrado = false;
    return resultado;
}

inline ResultadoIA resolver_profundidade(cubo inicial, int profundidademax= 11)
{
    ResultadoIA resultado;
    resultado.estadosVisitados=0;

    auto movimentos12=obter_movimentos();

    for(int limite=0; limite<=profundidademax; limite++)
    {
        std::stack <NoBusca> pilha;
        std::unordered_map <std::string, int> visitadosProfundidade;

        pilha.push({inicial, {}});
        visitadosProfundidade[inicial.obter_chave()]=0;

        while (!pilha.empty())
        {
            NoBusca atual=pilha.top();
            pilha.pop();

            resultado.estadosVisitados++;

            if(atual.estadoBusca.verificador()==true)
            {
                resultado.passos=atual.historicomovimentos;
                resultado.encontrado=true;
                return resultado;
            }

            if((int)atual.historicomovimentos.size()<limite)
            {
                for(const auto& m: movimentos12)
                {
                    cubo proximo=atual.estadoBusca;
                    (proximo.*(m.funcao))();

                    std::string chave = proximo.obter_chave();
                    int novaProfundidade = (int)atual.historicomovimentos.size()+1;

                    if (visitadosProfundidade.find(chave)==visitadosProfundidade.end() || novaProfundidade<visitadosProfundidade[chave])
                    {
                        visitadosProfundidade[chave]=novaProfundidade;
                        std::vector<std::string> novoHistorico = atual.historicomovimentos;
                        novoHistorico.push_back(m.nome);
                        pilha.push({proximo,novoHistorico});
                    }
                }
            }
        }
    }
    resultado.encontrado = false;
    return resultado;
}


#endif