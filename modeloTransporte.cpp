#include <ilcplex/ilocplex.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
ILOSTLBEGIN

const int num_of_threads = 2;

int main () {

	IloEnv env;
	stringstream logfile;
	try{
        IloModel modelo(env);
        IloInt m,n,check;
        //ler os dados do arquivo.
        ifstream entrada("in.txt");
        entrada >> m;
        entrada >> n;
        IloArray<IloNumArray> C(env,m);
        for(int i=0;i<m;i++){
            C[i]=IloNumArray(env,n);
        }
        IloNumArray O(env,m);
        IloNumArray D(env,n);

        //leitura dos dados.
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                entrada >> C[i][j];
            }

        }
        for(int i=0;i<m;i++){
            entrada >> O[i];
        }
        for(int j=0;j<n;j++){
            entrada >> D[j];
        }
        entrada >> check;
        if(check==777){
            cout << "Os arquivos foram carregados com sucesso!!!"<<endl;

        }else{
            cout<<"Erro de leitura!!!"<<endl;
        }
        IloArray<IloNumVarArray> x(env,m);
        //variaveis de decisão;
        for(int i=0;i<m;i++){
            x[i]=IloNumVarArray(env,n,0,IloInfinity);
        }
        IloExpr fo(env);
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                fo+=C[i][j]*x[i][j];

            }
        }
        cout << fo;
        modelo.add(IloMinimize(env,fo));
        //restrições
        IloExpr expressao(env);
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                expressao+=x[i][j];
            }
            modelo.add(expressao<=O[i]);
            expressao.end();
            expressao=IloExpr(env);
        }
        IloExpr expressao2(env);
        for(int j=0;j<n;j++){
            for(int i=0;i<m;i++){
                expressao2+=x[i][j];
            }
            modelo.add(expressao2>=D[j]);
            expressao2.end();
            expressao2=IloExpr(env);
        }

        IloCplex cplex ( env );
		cplex.setOut(env.getNullStream()); // omitir problemas
		cplex.extract(modelo);
		cplex.exportModel ( "modelo.lp" );
        env.out() << "Variaveis binarias: " << cplex.getNbinVars() << endl;
        env.out() << "Variaveis inteiras: " << cplex.getNintVars() << endl;
		env.out() << "Filas - Restricoes: " << cplex.getNrows() << endl;
        env.out() << "Colunas - Variaveis: " << cplex.getNcols() << endl;

        if ( !cplex.solve() ) {
            env.error() << "NAO EH POSSIVEL RESOLVER" << endl;
            throw ( -1 );
        }

		env.out() << "EH OTIMO ? = " << cplex.getStatus() << endl;
        env.out() << "Valor OBJETIVO = " << cplex.getObjValue() << endl;
        env.out() << "Tempo de execucao   = " << env.getTime() << endl;

		for(int i=0;i<m;i++)
			for(int j=0;j<n;j++)
				env.out() << "x[" << i+1 << "][" << j+1 << "]=" << cplex.getValue(x[i][j]) << endl;

    } catch (IloException& ex) {
        cerr << "Error Cplex: " << ex << endl;
    } catch (...) {
        cerr << "Error Cpp" << endl;
    }
    env.end();

	return 0;
}

