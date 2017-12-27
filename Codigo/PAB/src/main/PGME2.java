package main;

import java.util.HashMap;

import javax.naming.LimitExceededException;

import gurobi.GRB;
import gurobi.GRBEnv;
import gurobi.GRBException;
import gurobi.GRBLinExpr;
import gurobi.GRBModel;
import gurobi.GRBVar;

public class PGME2 {
	
	//Constantes
	private Integer[] n;
	private Integer[] m;
	private String[] k;
	private Integer[] l;
	private Integer[] v;
	private Integer[] d;
	private Integer[] a;
	private HashMap<String, Double> e;
	private HashMap<String, Double> c;
	private Double[][] h;
	private HashMap<String, Double>[] q;
	
	//Variaveis
	GRBVar[][][] y;
	
	
	
	public PGME2(Integer[] n, Integer[] m, Integer[] l, Integer[] v, Integer[] d, Integer[] a, String[] k,
			 HashMap<String, Double> e,  HashMap<String, Double> c,  HashMap<String, Double>[] q, Double[][] h) {
		
		this.n = n;
		this.m = m;
		this.k = k;
		this.l = l;
		this.v = v;
		this.d = d;
		this.a = a;
		this.e = e;
		this.c = c;
		this.q = q;
		this.h = h;

		try {
			GRBEnv env = new GRBEnv("otimizacao.log");
			GRBModel model = new GRBModel(env);
			
			criaVariaveis(model);
			criaFO(model);
			criaConstrants(model);
		
			model.optimize();
			
			for(int i : n) {
				for(int j : m) {
					for(int w : l) {
						if(y[i][j][w].get(GRB.DoubleAttr.X) == 1.0) {
							System.out.print(y[i][j][w].get(GRB.DoubleAttr.X)+" YN"+Integer.toString(i)+"M"+Integer.toString(j)+"B"+Integer.toString(w));
							System.out.println();
						}
					}
				}
			}
		} catch (GRBException exc) {
			exc.printStackTrace();
		}
	
		
	}
	
	public void criaVariaveis(GRBModel model) {
		
		y = new GRBVar[n[n.length-1]+1][m[m.length-1]+1][l[l.length-1]+1];
		
		try {
			for(int i : n) {
				for(int j : m) {
					for(int w : l) {
						y[i][j][w] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, "yI"+Integer.toString(i)+"J"+Integer.toString(j)+"L"+Integer.toString(w));
					}
				}
			}	
		
		} catch (GRBException e) {
			e.printStackTrace();
		}
	}	
	
	private void criaConstrants(GRBModel model) throws GRBException {
		
		GRBLinExpr expr;
		
		//4.3
		for(int i : n) {
			expr = new GRBLinExpr();
			for(int j=1; j<=a[i]-1; j++) {
				for(int w : l){
					expr.addTerm(1.0, y[i][j][w]);
				}
			}
			model.addConstr(0.0, GRB.EQUAL, expr, "4.3N"+Integer.toString(i));
		}
		
		//4.4
		for(int i : n) {
			expr = new GRBLinExpr();
			for(int j=a[i]; j<=m[m.length-1]; j++) {
				for(int w : l){
					expr.addTerm(1.0, y[i][j][w]);
				}
			}
			model.addConstr(1.0, GRB.EQUAL, expr, "4.4N"+Integer.toString(i));
		} 	

		//4.5
		for(int i : n) {
			for(int j : m) {
				for(int w : l) {
					expr = new GRBLinExpr();
					
					for(int i2 : n) {
						if(i2!=i) {
							for(int k=j; k<=j+h[i][w]-1; k++) {
								if(k>m[m.length-1]) 
									continue;
								expr.addTerm(1.0, y[i2][k][w]);								
							}
						}
					}
					
					GRBLinExpr expr2 = new GRBLinExpr();
					
					expr2.addConstant(n[n.length-1]);
					expr2.addTerm(n[n.length-1]*-1, y[i][j][w]);
				
					model.addConstr(expr, GRB.LESS_EQUAL, expr2, "4.5N"+Integer.toString(i)+"M"+Integer.toString(j)+"L"+Integer.toString(w));
				}
			}
		}
		
		//4.6
		for(int j : m) {
			for(String p : k) {
				expr = new GRBLinExpr();
				
				for(int i : n) {
					for(int b : l) {
						for(int z=a[i]; z<=j; z++) {
							expr.addTerm((Math.min(j-a[i]+1, h[i][b])/h[i][b])*q[i].get(p), y[i][z][b]);
						}
					}
				}
				model.addConstr(expr, GRB.GREATER_EQUAL, j*c.get(p) - e.get(p),"4.6M"+Integer.toString(j)+"K"+p );
			}	
		}
	}
	
	public void criaFO(GRBModel model) throws GRBException {
		
		GRBLinExpr expr = new GRBLinExpr();
		
		for(int i : n) {
			for(int j : m) {
				for(int w : l) {
					expr.addTerm(j-a[i]+h[i][w], y[i][j][w]);
				}
			}
		}
		
		model.setObjective(expr, GRB.MINIMIZE);
	}
}

