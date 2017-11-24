package main;

import dominio.Equipamento;
import dominio.Navio;
import dominio.Periodo;
import dominio.Rota;
import dominio.RotasConflito;
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
	private RotasConflito[] rc;
	
	//Variaveis
	private GRBVar[][][] f, e, ip, x;
	private GRBVar[][] ir;
	private GRBVar[][][][] sf, y, z, t, u;
	private GRBVar[][][][][][][] o;
	
	public Otimizacao(SubArea[] subAreas, Equipamento[] equipamentos, Periodo[] periodos, Rota[] rotas,
			Integer[] produtos,	Integer[] piers, RotasConflito[] rc) {
		
		this.subAreas = subAreas;
		this.equipamentos = equipamentos;
		this.periodos = periodos;
		this.rotas = rotas;
		this.produtos = produtos;
		this.piers = piers;
		this.rc = rc;
		
		try {
			GRBEnv env = new GRBEnv("otimizacao.log");
			GRBModel model = new GRBModel(env);
			
			criaVariaveis(model);
			criaFO(model);
			criaConstraints(model);
			
			model.optimize();
		} catch (GRBException e) {
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
		z = new GRBVar[rotasPP][produtos.length][produtos.length][periodos.length];//Variavel 5 na tese
		e = new GRBVar[subAreas.length][produtos.length][periodos.length];//Variavel 6 na tese
		ir = new GRBVar[produtos.length][periodos.length];//Variavel 7 na tese
		ip = new GRBVar[piers.length][produtos.length][periodos.length];//Variavel 8 na tese
		t = new GRBVar[rotas.length][produtos.length][produtos.length][periodos.length];//Variavel 9 na tese
		u = new GRBVar[rotas.length][produtos.length][produtos.length][periodos.length];//Variavel 10 na tese
		//o = new GRBVar[rotas.length][rotas.length][produtos.length][produtos.length][produtos.length][produtos.length][periodos.length];//Variavel 11 na tese
		
		//Cria a primeira variavel
		for(int s=0; s<subAreas.length; s++) {
			for(int p=0; p<produtos.length; p++) {
				for(int t=0; t<periodos.length; t++) {
					name = "fS"+Integer.toString(s)+"P"+Integer.toString(p)+"T"+Integer.toString(t);
					f[s][p][t] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, name);
				}
			}
		}
		//Cria a segunda variavel
		for(int s=0; s<subAreas.length; s++) {
			for(int p=1; p<produtos.length; p++) {
				for(int pl=0; pl<produtos.length; pl++) {
					for(int t=0; t<periodos.length; t++) {
						name = "sfS"+Integer.toString(s)+"P"+Integer.toString(p)+"P'"+Integer.toString(pl)+"T"+Integer.toString(t);
						sf[s][p][pl][t] = model.addVar(0.0, 1.0, 0.0, GRB.CONTINUOUS, name );
					}
				}
			}
		}
		
		int rotasX = 0;
		//Cria a variavel 3
		for(int r=0; r<rotas.length; r++) {
			if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
				for(int p=1; p<produtos.length; p++) {
					for(int t=0; t<periodos.length; t++) {
						x[rotasX][p][t] = model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "xR"+Integer.toString(r)+"P"+Integer.toString(p)+"T"+Integer.toString(t));
					}
				}
				rotasX++;
			}
		}
		int rotasY = 0;
		//Cria a variavel 4
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							y[rotasY][j][k][w] =  model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "yR"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T'"+Integer.toString(w));						
						}
					}
				}
				rotasY++;	
			}
		}	
		int rotasZ = 0;
		//Cria a variavel 5
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.PATIO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							z[rotasZ][j][k][w] =  model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "zR"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T'"+Integer.toString(w));
						}
					}
				}
				rotasZ++;	
			}
		}	

		//Cria a variavel 6
		for(int s=0; s<subAreas.length; s++) {
			for(int p=0; p<produtos.length; p++) {
				for(int t=0; t<periodos.length; t++) {
					name = "eS"+Integer.toString(s)+"P"+Integer.toString(p)+"T"+Integer.toString(t);
					e[s][p][t] = model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, name);
				}
			}
		}	
		//Cria variavel 7
		for(int p=0; p<produtos.length;p++) {
			for(int t=0; t<periodos.length; t++) {
				int quant = 0;//soma da quantidade do produtos no periodo e nos anteriores
				for(int w=t;w>=0;w--) {//percorre os periodos anteriodes
					if(periodos[w].getChegadaProdutos().containsKey(p)) {
						quant += periodos[w].getChegadaProdutos().get(p);
					}
				}
				ir[p][t] = model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, "irP"+Integer.toString(p)+"T"+Integer.toBinaryString(t));
			}
		}
		
		//Cria variavel 8
		for(int n=0;n<piers.length; n++) {
			for(int t=0; t<periodos.length; t++) {
				for(int p=1;p<produtos.length; p++) {
					name = "ipN"+Integer.toString(n)+"P"+Integer.toString(p)+"T"+Integer.toString(t);
					ip[n][p][t] = model.addVar(0.0, GRB.INFINITY, 1.0, GRB.CONTINUOUS, name);
				}				
			}
		}	
		//Cria variavel 9
		for(int i=0; i<rotas.length;i++) {
			for(int j=1; j<produtos.length;j++) {
				for(int k=1; k<produtos.length;k++) {
					for(int w=0; w<periodos.length; w++) {
						name = "tS"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w);
						if(((rotas[i].getTipo()==Rota.RECEPCAO_PATIO)&&(j!=k))) {
							continue; 
						}
						else {
							t[i][j][k][w] = model.addVar(0.0, GRB.INFINITY, 0.0, GRB.CONTINUOUS, name);
							
						}
					}
				}
			}
		}
		//Cria a variavel 10
		for(int i=0; i<rotas.length;i++) {
			for(int j=1; j<produtos.length;j++) {
				for(int k=1; k<produtos.length;k++) {
					for(int w=0; w<periodos.length; w++) {
						name = "uS"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w);
						if((rotas[i].getTipo()==Rota.RECEPCAO_PATIO)&&(j!=k)) {
							continue;						
						}
						else {
							u[i][j][k][w] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, name);
							
						}
					}
				}
			}
		}
		
		/*//Cria a variavel 11
		for(int r=0; r<rc.length; r++) {
			for(int p=1; p<produtos.length; p++) {
				for(int pl=1; pl<produtos.length; pl++) {
					for(int pc=1; pc<produtos.length; pc++) {
						for(int pcl=1; pcl<produtos.length; pcl++) {
							for(int t=0; t<periodos.length; t++) {
								name = "oR"+Integer.toString(rc[r].getR1().getId())+"RL"+Integer.toString(rc[r].getR2().getId())+"P"+Integer.toString(p)+"PL"+Integer.toString(pl)+"PC"+Integer.toString(pc)+"PCL"+Integer.toString(pcl)+"T"+Integer.toString(t);
								o[rc[r].getR1().getId()][rc[r].getR2().getId()][p][pl][pc][pcl][t] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, name);
							}
						}
					}
				}
			}
		}*/
	}
	
	private void criaConstraints(GRBModel model) throws GRBException {
		
		GRBLinExpr expr;
		String name;
		int auxRotas, rRPa, rRPi, rPP;
		
		//Constraint 4.2
		for(int p=1; p<produtos.length; p++) {
			for(int t=0; t<periodos.length; t++) {
				expr = new GRBLinExpr();
				auxRotas=0;
				for(int r=0; r<rotas.length; r++) {
					if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
						expr.addTerm(rotas[r].getCapacidade(), x[auxRotas][p][t]);
						auxRotas++;
					}
				}
				auxRotas=0;
				for(int r=0; r<rotas.length; r++) {
					if(rotas[r].getTipo()==Rota.RECEPCAO_PIER) { 
						for(int pl=1; pl<produtos.length; pl++) {
							expr.addTerm(rotas[r].getCapacidade(), y[auxRotas][p][pl][t]);
						}
						auxRotas++;
					}
				}
				if(t>0) {
					expr.addTerm(-1.0, ir[p][t-1]);
				}
				expr.addTerm(1.0, ir[p][t]);
				int a = 0;
				if(periodos[t].getChegadaProdutos().containsKey(p)) {
					a = periodos[t].getChegadaProdutos().get(p);
				}
				model.addConstr(expr, GRB.EQUAL, a, "4.2P"+Integer.toString(p)+"T"+Integer.toString(t));
			}
		}

		//Constraint 4.3
		for(int n=0; n<piers.length; n++) {
			for(int p=1; p<produtos.length; p++) {
				for(int t=0; t<periodos.length; t++) {
					expr = new GRBLinExpr();
					auxRotas = 0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.PATIO_PIER) {
							for(int pl=1; pl<produtos.length; pl++) {
								expr.addTerm(rotas[r].getCapacidade(), z[auxRotas][p][pl][t]);
							}
							auxRotas++;
						}
					}
					auxRotas=0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PIER) {
							for(int pl=1; pl<produtos.length; pl++) {
								expr.addTerm(rotas[r].getCapacidade(), y[auxRotas][p][pl][t]);
							}
							auxRotas++;
						}
					}
					if(t>0) {
						expr.addTerm(-1.0, ip[n][p][t-1]);						
					}
					expr.addTerm(1.0, ip[n][p][t]);
					Navio[] navios = periodos[t].getNavios();
					int d =0;
					for(int i=0; i<navios.length; i++) {
						if(navios[i].getPier()==n) {
							if(navios[i].getProdutosQuantidade().containsKey(p)) {
								d += navios[i].getProdutosQuantidade().get(p);
							}
						}
					}
					model.addConstr(expr, GRB.EQUAL, d, "4.3N"+Integer.toString(n)+"P"+Integer.toString(p)+"T"+Integer.toString(t));
					//model.write("grb.lp");
				}
			}
		}
		
		//Constraint 4.4
		for(int s=0; s<subAreas.length; s++) {
			for(int p=1; p<produtos.length; p++) {
				for(int t=0	; t<periodos.length-1; t++) {
					expr = new GRBLinExpr();
					expr.addTerm(1.0, e[s][p][t]);
					auxRotas = 0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
							expr.addTerm(rotas[r].getCapacidade(), x[auxRotas][p][t]);
							auxRotas++;
						}
					}
					auxRotas = 0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.PATIO_PIER) {
							for(int pl=1; pl<produtos.length; pl++) {
								expr.addTerm(rotas[r].getCapacidade()*(-1), z[auxRotas][p][pl][t]);
							}
							auxRotas++;
						}
					}
					model.addConstr(e[s][p][t+1], GRB.EQUAL, expr, "4.4S"+Integer.toString(s)+"P"+Integer.toString(p)+"T"+Integer.toString(t));
				}
			}
		}
		
		//Constraint 4.5
		for(int i=0; i<subAreas.length; i++) {
			for(int j=0; j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					model.addConstr(e[i][j][k], GRB.LESS_EQUAL, subAreas[i].getCapacidade(), "4.5S"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k));
				}
			}
		}
		
		//Constraint 4.6
		for(int m=0; m<equipamentos.length; m++) {
			for(int t=0; t<periodos.length; t++) {
				expr = new GRBLinExpr();
				for(int p=1; p<produtos.length; p++) {
					rRPa = rRPi = rPP = 0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
							expr.addTerm(rotas[r].getCapacidade(), x[rRPa][p][t]);
							rRPa++;
						}
						
						if(rotas[r].getTipo()==Rota.RECEPCAO_PIER) {
							for(int pl=1; pl<produtos.length; pl++) {
								expr.addTerm(rotas[r].getCapacidade(), y[rRPi][p][pl][t]);
							}
							rRPi++;
						}
						
							
						if(rotas[r].getTipo()==Rota.PATIO_PIER) {
							for(int pl=1; pl<produtos.length; pl++) {
								expr.addTerm(rotas[r].getCapacidade(), z[rPP][p][pl][t]);
							}
							rPP++;
						}
					}
				}
				model.addConstr(expr, GRB.LESS_EQUAL, Periodo.TEMPO_PERIODO*equipamentos[m].getCapacidadeTransporte(), "4.6M"+Integer.toString(m)+"T"+Integer.toString(t));
			}
		}
		
		//Constraint 4.7
		for(int i=0; i<subAreas.length; i++) {
			for(int w=0; w<periodos.length;w++) {
				expr = new GRBLinExpr();
				for(int j=0; j<produtos.length;j++) {
					expr.addTerm(1.0, f[i][j][w]);
				}
				model.addConstr(expr, GRB.EQUAL, 1.0, "4.7-S"+Integer.toString(i)+"T"+Integer.toString(w));
			}
		}
		
		//Constraint 4.8
		for (int i=0; i<subAreas.length;i++) {
			for(int w=0; w<periodos.length; w++) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=0; k<produtos.length; k++) {
						name = "4.8S"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w);
						if((j==0) || (w==0) || (j==k)) {
							model.addConstr(0.0, GRB.EQUAL, sf[i][j][k][w], name);
						}
						else {
							expr = new GRBLinExpr();
							expr.addTerm(1.0, f[i][j][w-1]);
							expr.addTerm(1.0, f[i][k][w]);
							expr.addTerm(-1.0, sf[i][j][k][w]);
							model.addConstr(expr, GRB.LESS_EQUAL, 1.0, name);
						}
					}	
				}
			}
		}
		
		//Constraint 4.9
		for(int i=0; i<subAreas.length; i++) {
			for(int j=1; j<produtos.length; j++) {
				for(int k=0; k<periodos.length; k++) {
					name = "4.9S"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k);
					expr = new GRBLinExpr();
					expr.addTerm(subAreas[i].getCapacidade(), f[i][j][k]);
					expr.addTerm(-1.0, e[i][j][k]);
					model.addConstr(expr, GRB.GREATER_EQUAL, 0.0, name);
				}
			}
		}
		
		//Constraint 4.10
		for(int s=0; s<subAreas.length; s++) {
			for(int p=1; p<produtos.length;p++) {
				for(int t=0; t<periodos.length;t++) {
					expr = new GRBLinExpr();
					auxRotas = 0;
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
							expr.addTerm(-1*rotas[r].getCapacidade(), x[auxRotas][p][t]);
							auxRotas++;
						}
					}	
					expr.addTerm(subAreas[s].getCapacidade(), f[s][p][t]);
					model.addConstr(expr, GRB.GREATER_EQUAL, 0.0, "4.10S"+Integer.toString(s)+"P"+Integer.toString(p)+"T"+Integer.toString(t));
				}
			}
		}
		
		//Constraint 4.11
		int rotasAux =0;
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PATIO) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=0; k<periodos.length; k++) {
						expr = new GRBLinExpr();
						expr.addTerm(Periodo.TEMPO_PERIODO, u[i][j][j][k]);
						model.addConstr(expr, GRB.GREATER_EQUAL, x[rotasAux][j][k], "4.11R"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k));
					}
				}
				rotasAux++;
			}
		}
		
		rotasAux = 0;
		//Constraint 4.12
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(Periodo.TEMPO_PERIODO, u[i][j][k][w]);
							model.addConstr(expr, GRB.GREATER_EQUAL, y[rotasAux][j][k][w], "4.12R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
				rotasAux++;
			}
		}
		
		rotasAux = 0;
		//Constraint 4.13
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.PATIO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(Periodo.TEMPO_PERIODO, u[i][j][k][w]);
							model.addConstr(expr, GRB.GREATER_EQUAL, z[rotasAux][j][k][w], "4.13R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
			rotasAux++;
			}
		}
		
		//Constraint 4.26
		rotasAux=0;
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PATIO) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=0; k<periodos.length; k++) {
						expr = new GRBLinExpr();
						expr.addTerm(1.0, t[i][j][j][k]);
						expr.addTerm(1.0, x[rotasAux][j][k]);
						model.addConstr(expr, GRB.LESS_EQUAL, (k+1)*Periodo.TEMPO_PERIODO, "4.26R"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k));
					}
				}
			rotasAux++;
			}
		}

		//Constraint 4.27
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PATIO) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=0; k<periodos.length; k++) {
						expr = new GRBLinExpr();
						expr.addTerm(1.0, t[i][j][j][k]);
						model.addConstr(expr, GRB.GREATER_EQUAL, k*Periodo.TEMPO_PERIODO, "4.27R"+Integer.toString(i)+"P"+Integer.toString(j)+"T"+Integer.toString(k));
					}
				}
			}
		}
		
		//Constraint 4.28
		rotasAux=0;
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(1.0, t[i][j][k][w]);
							expr.addTerm(1.0, y[rotasAux][j][k][w]);
							model.addConstr(expr, GRB.LESS_EQUAL, (w+1)*Periodo.TEMPO_PERIODO, "4.28R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
				rotasAux++;
			}
		}
		
		//Constraint 4.29
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.RECEPCAO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(1.0, t[i][j][k][w]);
							model.addConstr(expr, GRB.GREATER_EQUAL, w*Periodo.TEMPO_PERIODO, "4.29R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
			}
		}
		
		//Constraint 4.30
		rotasAux=0;
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.PATIO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(1.0, t[i][j][k][w]);
							expr.addTerm(1.0, z[rotasAux][k][k][w]);
							model.addConstr(expr, GRB.LESS_EQUAL, (w+1)*Periodo.TEMPO_PERIODO, "4.28R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
				rotasAux++;
			}
		}
		
		//Constraint 4.31
		for(int i=0; i<rotas.length; i++) {
			if(rotas[i].getTipo()==Rota.PATIO_PIER) {
				for(int j=1; j<produtos.length; j++) {
					for(int k=1; k<produtos.length; k++) {
						for(int w=0; w<periodos.length; w++) {
							expr = new GRBLinExpr();
							expr.addTerm(1.0, t[i][j][k][w]);
							model.addConstr(expr, GRB.GREATER_EQUAL, w*Periodo.TEMPO_PERIODO, "4.29R"+Integer.toString(i)+"P"+Integer.toString(j)+"P'"+Integer.toString(k)+"T"+Integer.toString(w));
						}
					}
				}
			}
		}
		
	}

	private void criaFO(GRBModel model) {
		
		GRBLinExpr expr = new GRBLinExpr();
		
		for(int p=1; p<produtos.length; p++) {
			for(int t=0; t<periodos.length; t++) {
				expr.addTerm(2.0, ir[p][t]);
			}
		}
		
		int b=10; 
		for(int n=0; n<2; n++) {
			for(int p=1; p<produtos.length; p++) {
				for(int t=0; t<periodos.length; t++) {
					expr.addTerm(b, ip[n][p][t]);
				}
			}
			b+=40;
		}
		
		for(int s=0; s<subAreas.length; s++) {
			for(int p=1; p<produtos.length; p++) {
				for(int pl=0; pl<produtos.length; pl++) {
					for(int t=0; t<periodos.length; t++) {
						expr.addTerm(10.0, sf[s][p][pl][t]);
					}
				}
			}
		}
		
		int auxY = 0;
		int auxZ = 0;
		for(int p=1; p<produtos.length;p++) {
			for(int pl=1; pl<produtos.length; pl++) {
				for(int t=0; t<periodos.length;t++) {
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PIER) {
							expr.addTerm(0.01*rotas[r].getCapacidade(), y[auxY][p][pl][t]);
							auxY++;
						}
						if(rotas[r].getTipo()==Rota.PATIO_PIER) {
							expr.addTerm(0.01*rotas[r].getCapacidade(), z[auxZ][p][pl][t]);
							auxZ++;
						}
					}
					auxY=0;
					auxZ=0;
				}
			}
		}

		int auxX = 0;
		auxY = 0;
		auxZ = 0;
		for(int p=1; p<produtos.length; p++) {
			for(int t=0; t<periodos.length; t++) {
				for(int r=0; r<rotas.length; r++) {
					if(rotas[r].getTipo()==Rota.RECEPCAO_PATIO) {
						expr.addTerm(rotas[r].getCapacidade(), x[auxX][p][t]);
						auxX++;
					}
				}
				auxX = 0;
			}
		}
		
		for(int p=1; p<produtos.length; p++) {
			for(int pl=1; pl<produtos.length; pl++) {
				for(int t=0; t<periodos.length; t++) {
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.RECEPCAO_PIER) {
							expr.addTerm(rotas[r].getCapacidade(), y[auxY][p][pl][t]);
							auxY++;
						}
					}
					auxY = 0;
				}
			}
		}
		
		for(int p=1; p<produtos.length; p++) {
			for(int pl=1; pl<produtos.length; pl++) {
				for(int t=0; t<periodos.length; t++) {
					for(int r=0; r<rotas.length; r++) {
						if(rotas[r].getTipo()==Rota.PATIO_PIER) {
							expr.addTerm(rotas[r].getCapacidade(), z[auxZ][p][pl][t]);
							auxZ++;
						}
					}
					auxZ = 0;
				}
			}
		}
		try {
			model.setObjective(expr, GRB.MINIMIZE);
		} catch (GRBException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
