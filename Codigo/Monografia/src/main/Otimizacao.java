package main;

import dominio.Equipamento;
import dominio.Navio;
import dominio.Periodo;
import dominio.Rota;
import dominio.SubArea;

import gurobi.GRB;
import gurobi.GRBEnv;
import gurobi.GRBException;
import gurobi.GRBModel;
import gurobi.GRBVar;
import gurobi.GRBLinExpr;

public class Otimizacao {

	private SubArea[] subAreas;
	private Equipamento[] equipamentos;
	private Periodo[] periodos;
	private Rota[] rotas;
	private Integer[]  piers, produtos;

	//Variaveis
	private GRBVar[][][] f, e, ip, x;
	private GRBVar[][] ir;
	private GRBVar[][][][] sf, y;
	
	public Otimizacao(SubArea[] subAreas, Equipamento[] equipamentos, Periodo[] periodos, Rota[] rotas,
			Integer[] produtos,	Integer[] piers) {
		
		this.subAreas = subAreas;
		this.equipamentos = equipamentos;
		this.periodos = periodos;
		this.rotas = rotas;
		this.produtos = produtos;
		this.piers = piers;
				
		try {
			GRBEnv env = new GRBEnv("otimizacao.log");
			GRBModel model = new GRBModel(env);
			
			criaVariaveis(model);
			criaConstraints(model);
		} catch (GRBException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	private void criaVariaveis(GRBModel model) throws GRBException{
						
		int rotasRPa = 0;
		int rotasPP = 0;
		int rotasRPi = 0;
		
		for(int i=0; i<rotas.length; i++) {
			switch(rotas[i].getTipo()) {
				case(Rota.RECEPCAO_PIER):
					rotasRPi++;
					break;
				case(Rota.RECEPCAO_PATIO):
					rotasRPa++;
					break;
				case(Rota.PATIO_PIER):
					rotasPP++;
					break;
				default:
					break;
			}
		}
		
		String name;
		
		f = new GRBVar[subAreas.length][produtos.length][periodos.length];//Variavel 1 na tese
		sf = new GRBVar[subAreas.length][produtos.length][produtos.length][periodos.length];//Variavel 2 na tese
		x = new GRBVar[rotasRPa][produtos.length][periodos.length];//Variavel 3 na tese
		y = new GRBVar[rotasRPi][produtos.length][produtos.length][periodos.length];//Variavel 4 na tese
		e = new GRBVar[subAreas.length][produtos.length][periodos.length];//Variavel 6 na tese
		ir = new GRBVar[produtos.length][periodos.length];//Variavel 7 na tese
		ip = new GRBVar[piers.length][produtos.length][periodos.length];//Variavel 8 na tese
		
		//Cria a primeira variavel
		for(int i=0; i<subAreas.length; i++) {
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					name = "fS"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k);
					if(k==0) {
						if((j==subAreas[i].getProdutoEstocado())) {
							f[i][j][k] = model.addVar(1.0, 1.0, 0.0, GRB.BINARY,name);
						}
						else {
							f[i][j][k]=model.addVar(0.0, 0.0, 0.0, GRB.BINARY, name);
						}
					}
					else {
						f[i][j][k]=model.addVar(0.0, 1.0, 0.0, GRB.BINARY, name);
					}
							
				}
			}
		}
		
		//Cria a segunda variavel
		for(int i=0; i<subAreas.length; i++) {
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<produtos.length; k++) {
					for(int w=0; w<periodos.length; w++) {
						name = "sfS"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w);
						if((w==0) ||(j==k) || (j==0)) {
							sf[i][j][k][w] = model.addVar(0.0, 0.0, 0.0, GRB.CONTINUOUS, name );
						}
						else {
							sf[i][j][k][w] = model.addVar(0.0, 1.0, 0.0, GRB.CONTINUOUS, name);
						}
					}
				}
			}
		}
		
		
		
		//Cria a variavel 3
		for(int i=0; i<rotas.length; i++) {
			int rotasAux = 0;
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					if(rotas[i].getTipo()==Rota.RECEPCAO_PATIO) {
						x[rotasAux][j][k] = model.addVar(0.0, Periodo.TEMPO_PERIODO, 0.0, GRB.CONTINUOUS, "xR"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k));
						rotasAux++;
					}
				}
			}
		}
		
	
		//Cria a variavel 4
		for(int i=0; i<rotas.length; i++) {
			int rotasAux = 0;
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<produtos.length; k++) {
					for(int w=0; k<periodos.length; w++) {
						if(rotas[i].getTipo()==Rota.RECEPCAO_PIER) {
							y[rotasAux][j][k][w] =  model.addVar(0.0, Periodo.TEMPO_PERIODO, 0.0, GRB.CONTINUOUS, "yR"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T'"+Integer.toString(w));
							rotasAux++;
						}
					}
				}
			}
		}	
	
	
		//Cria a variavel 6
		for(int i=0; i<subAreas.length; i++) {
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					name = "eS"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k);
					if(k==0) {
						if(j==subAreas[i].getProdutoEstocado()) {
							e[i][j][k] = model.addVar(subAreas[i].getQuantidadeEstocada(), subAreas[i].getQuantidadeEstocada(), 0.0, GRB.CONTINUOUS, name);
						}
						else {
							e[i][j][k] = model.addVar(0.0, 0.0, 0.0, GRB.CONTINUOUS, name);
						}
					}
					else {
						e[i][j][k] = model.addVar(0.0, subAreas[i].getCapacidade(), 0.0, GRB.CONTINUOUS, name);
					}
				}
			}
		}	
		
		//Cria variavel 7
		for(int i=0;i<produtos.length;i++) {
			for(int j=0; j<periodos.length; j++) {
				int quant = 0;//soma da quantidade do produtos no periodo e nos anteriores
				for(int w=j;w>=0;w--) {//percorre os periodos anteriodes
					if(periodos[w].getChegadaProdutos().containsKey(i)) {
						quant += periodos[w].getChegadaProdutos().get(j);
					}
				}
				ir[i][j] = model.addVar(0.0, quant, 0.0, GRB.CONTINUOUS, "irP"+Integer.toString(i)+"T"+Integer.toBinaryString(j));
			}
		}
		
		
		//Cria variavel 8
		for(int i=0;i<piers.length; i++) {
			for(int j=0;j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					Navio[] navios = periodos[k].getNavios();
					boolean temNavio = false;
					int navio =-1;
					for(int w=0;i<navios.length;w++) {//verifica se algum navio vai atracar no pier i no periodo k
						if(navios[w].getPier()==i) {
							temNavio = true;
							navio = k;
							break;
						}
					}
					if(temNavio) {//se algum navio vai atracar
						if(navios[navio].getProdutosQuantidade().containsKey(j)) {
							ip[i][j][k] = model.addVar(0.0, navios[navio].getProdutosQuantidade().get(j), 0.0, GRB.INTEGER,"ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(k));
						}
						else {
							ip[i][j][k]=model.addVar(0.0, 0.0, 0.0, GRB.INTEGER,"ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(k));
						}
					}
					else {
						ip[i][j][k] = model.addVar(0.0, 0.0, 0.0, GRB.INTEGER, "ip"+Integer.toString(i)+Integer.toString(j)+Integer.toString(k));
					}
				}				
			}
		}		
	}
	
	private void criaConstraints(GRBModel model) throws GRBException {
		
		GRBLinExpr expr;
		
		//Constraint 4.7
		for(int i=0; i<f.length; i++) {
			for(int j=0; j<f[i].length;j++) {
				expr = new GRBLinExpr();
				for(int w=0; w<f[i][j].length;w++) {
					expr.addTerm(1.0, f[i][j][w]);
				}
				model.addConstr(expr, GRB.EQUAL, 1.0, "4.7-"+Integer.toString(i)+Integer.toString(j));
			}
		}
		
		//Constraint 4.8
		for (int i=0; i<sf.length;i++) {
			for(int j=0; j<sf[i].length; j++) {
				for(int k=0; k<sf[i][j].length; k++) {
					for(int w=0; w<sf[i][j][k].length; w++) {
						expr = new GRBLinExpr();
						if((j==0)|| (k==0) || (k==w)) {
							model.addConstr(0.0, GRB.EQUAL, sf[i][j][k][w], "4.8"+Integer.toString(i)+Integer.toString(j)+Integer.toString(k)+Integer.toString(w));
						}
						else {
							expr.addTerm(1.0, f[i][j-1][k]);
							expr.addTerm(1.0, f[i][j][w]);
							expr.addTerm(-1.0, sf[i][j][k][w]);
							model.addConstr(expr, GRB.LESS_EQUAL, 1.0, "4.8"+Integer.toString(i)+Integer.toString(j)+Integer.toString(k)+Integer.toString(w));
						}
					}	
				}
			}
		}	
	}
}
