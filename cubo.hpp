#ifndef CUBO_HPP
#define CUBO_HPP

#include <iostream>
#include <cstdlib>

// pensando que cada lado vai ser representado por faces[0-5][x][x]
// 0-TOPO
// 1-BASE
// 2-ESQUERDA
// 3-FRENTE
// 4-DIREITA
// 5-TRASEIRA

enum Face 
{
    TOPO = 0,
    BASE = 1,
    ESQUERDA = 2,
    FRENTE = 3,
    DIREITA = 4,
    TRASEIRA = 5
};


struct cubo
    {
        int faces[6][2][2];

        cubo()
        {
            for(int i=0;i<6;i++)
            {
                for(int j=0;j<2;j++)
                {
                    for(int k=0;k<2;k++)
                    {
                        faces[i][j][k] = i;
                    }
                }
            }
        }

        void imprimir()
        {
            
            for(int i=0;i<2;i++)
            {
                std::cout << "    " << faces[0][i][0] << "   " << faces[0][i][1] << "\n";
            }

            for(int i=0;i<2;i++)
            {
                for(int j=2;j<6;j++)
                {
                    std::cout << faces[j][i][0] << " " << faces[j][i][1] << "  ";
                }
                std::cout << "\n";
            }

            for(int i=0;i<2;i++)
            {
                std::cout << "    " << faces[1][i][0] << "   " << faces[1][i][1] << "\n";
            }
            std::cout << "\n";
        }

        bool verificador() const 
        {
            for (int i = 0; i < 6; i++) 
            {
            int corPadrao = faces[i][0][0]; 

            if (faces[i][0][1] != corPadrao || faces[i][1][0] != corPadrao || faces[i][1][1] != corPadrao) 
                {
                return false; 
                }
            }
    
        return true; 
        }
    
        void embaralhar(int quantidade=15)
        {
            for(int i=0;i<quantidade;i++)
            {
                int sorteio=rand()%6;
                switch (sorteio)
                {
                    case 0: girar_f(); break;
                    case 1: girar_r(); break;
                    case 2: girar_u(); break;
                    case 3: girar_l(); break;
                    case 4: girar_d(); break;
                    case 5: girar_b(); break;
                }
            }
        }

        void girar_f()
        {
            int tempf=faces[FRENTE][0][0];
            faces[FRENTE][0][0]=faces[FRENTE][1][0];
            faces[FRENTE][1][0]=faces[FRENTE][1][1];
            faces[FRENTE][1][1]=faces[FRENTE][0][1];
            faces[FRENTE][0][1]=tempf;

            int tempT0 = faces[TOPO][1][0];
            int tempT1 = faces[TOPO][1][1];

            faces[TOPO][1][0] = faces[ESQUERDA][1][1];
            faces[TOPO][1][1] = faces[ESQUERDA][0][1];

            faces[ESQUERDA][0][1] = faces[BASE][0][0];
            faces[ESQUERDA][1][1] = faces[BASE][0][1];

            faces[BASE][0][0] = faces[DIREITA][1][0];
            faces[BASE][0][1] = faces[DIREITA][0][0];

            faces[DIREITA][0][0] = tempT0;
            faces[DIREITA][1][0] = tempT1;
        }

        void girar_r() 
        {
            int tempR = faces[DIREITA][0][0];
            faces[DIREITA][0][0] = faces[DIREITA][1][0];
            faces[DIREITA][1][0] = faces[DIREITA][1][1];
            faces[DIREITA][1][1] = faces[DIREITA][0][1];
            faces[DIREITA][0][1] = tempR;

            int tempT0 = faces[TOPO][0][1];
            int tempT1 = faces[TOPO][1][1];

            faces[TOPO][0][1] = faces[FRENTE][0][1];
            faces[TOPO][1][1] = faces[FRENTE][1][1];

            faces[FRENTE][0][1] = faces[BASE][0][1];
            faces[FRENTE][1][1] = faces[BASE][1][1];

            faces[BASE][0][1] = faces[TRASEIRA][1][0];
            faces[BASE][1][1] = faces[TRASEIRA][0][0];

            faces[TRASEIRA][0][0] = tempT1;
            faces[TRASEIRA][1][0] = tempT0;
        }

        void girar_u() 
        {
            int tempU = faces[TOPO][0][0];
            faces[TOPO][0][0] = faces[TOPO][1][0];
            faces[TOPO][1][0] = faces[TOPO][1][1];
            faces[TOPO][1][1] = faces[TOPO][0][1];
            faces[TOPO][0][1] = tempU;

            int tempF0 = faces[FRENTE][0][0];
            int tempF1 = faces[FRENTE][0][1];

            faces[FRENTE][0][0] = faces[DIREITA][0][0];
            faces[FRENTE][0][1] = faces[DIREITA][0][1];

            faces[DIREITA][0][0] = faces[TRASEIRA][0][0];
            faces[DIREITA][0][1] = faces[TRASEIRA][0][1];

            faces[TRASEIRA][0][0] = faces[ESQUERDA][0][0];
            faces[TRASEIRA][0][1] = faces[ESQUERDA][0][1];

            faces[ESQUERDA][0][0] = tempF0;
            faces[ESQUERDA][0][1] = tempF1;
        }

            void girar_l() 
        {
            int tempL = faces[ESQUERDA][0][0];
            faces[ESQUERDA][0][0] = faces[ESQUERDA][1][0];
            faces[ESQUERDA][1][0] = faces[ESQUERDA][1][1];
            faces[ESQUERDA][1][1] = faces[ESQUERDA][0][1];
            faces[ESQUERDA][0][1] = tempL;

            int tempT0 = faces[TOPO][0][0];
            int tempT1 = faces[TOPO][1][0];

            faces[TOPO][0][0] = faces[TRASEIRA][1][1];
            faces[TOPO][1][0] = faces[TRASEIRA][0][1];

            faces[TRASEIRA][1][1] = faces[BASE][0][0];
            faces[TRASEIRA][0][1] = faces[BASE][1][0];

            faces[BASE][0][0] = faces[FRENTE][0][0];
            faces[BASE][1][0] = faces[FRENTE][1][0];

            faces[FRENTE][0][0] = tempT0;
            faces[FRENTE][1][0] = tempT1;
        }

        void girar_d() 
        {
            int tempD = faces[BASE][0][0];
            faces[BASE][0][0] = faces[BASE][1][0];
            faces[BASE][1][0] = faces[BASE][1][1];
            faces[BASE][1][1] = faces[BASE][0][1];
            faces[BASE][0][1] = tempD;

            int tempF0 = faces[FRENTE][1][0];
            int tempF1 = faces[FRENTE][1][1];

            faces[FRENTE][1][0] = faces[ESQUERDA][1][0];
            faces[FRENTE][1][1] = faces[ESQUERDA][1][1];

            faces[ESQUERDA][1][0] = faces[TRASEIRA][1][0];
            faces[ESQUERDA][1][1] = faces[TRASEIRA][1][1];

            faces[TRASEIRA][1][0] = faces[DIREITA][1][0];
            faces[TRASEIRA][1][1] = faces[DIREITA][1][1];

            faces[DIREITA][1][0] = tempF0;
            faces[DIREITA][1][1] = tempF1;
        }

        void girar_b() 
        {
            int tempB = faces[TRASEIRA][0][0];
            faces[TRASEIRA][0][0] = faces[TRASEIRA][1][0];
            faces[TRASEIRA][1][0] = faces[TRASEIRA][1][1];
            faces[TRASEIRA][1][1] = faces[TRASEIRA][0][1];
            faces[TRASEIRA][0][1] = tempB;

            int tempT0 = faces[TOPO][0][0];
            int tempT1 = faces[TOPO][0][1];

            faces[TOPO][0][0] = faces[DIREITA][0][1];
            faces[TOPO][0][1] = faces[DIREITA][1][1];

            faces[DIREITA][0][1] = faces[BASE][1][1];
            faces[DIREITA][1][1] = faces[BASE][1][0];

            faces[BASE][1][1] = faces[ESQUERDA][1][0];
            faces[BASE][1][0] = faces[ESQUERDA][0][0];

            faces[ESQUERDA][1][0] = tempT0;
            faces[ESQUERDA][0][0] = tempT1;
        }

        void girar_duplof()
        {
            girar_f(); girar_f();
        }

        void girar_antihorariof()
        {
            girar_f(); girar_f(); girar_f();
        }

        void girar_duplor()
        {
            girar_r(); girar_r();
        }

        void girar_antihorarior()
        {
            girar_r(); girar_r(); girar_r();
        }

        void girar_duplou()
        {
            girar_u(); girar_u();
        }

        void girar_antihorariou()
        {
            girar_u(); girar_u(); girar_u();
        }

        void girar_duplol()
        {
            girar_l(); girar_l();
        }

        void girar_antihorariol()
        {
            girar_l(); girar_l(); girar_l();
        }

        void girar_duplod()
        {
            girar_d(); girar_d();
        }

        void girar_antihorariod()
        {
            girar_d(); girar_d(); girar_d();
        }

        void girar_duplob()
        {
            girar_b(); girar_b();
        }

        void girar_antihorariob()
        {
            girar_b(); girar_b(); girar_b();
        }
    };
#endif
