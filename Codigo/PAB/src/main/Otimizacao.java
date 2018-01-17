package main;

import java.util.HashMap;

import gurobi.GRB;
import gurobi.GRBEnv;
import gurobi.GRBException;
import gurobi.GRBModel;
import gurobi.GRBVar;
import gurobi.GRBLinExpr;

public class Otimizacao {

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
	private  HashMap<String, Double>[] q;
	
	//Variaveis
	GRBVar[][] x;
	GRBVar[][] u;
	GRBVar[][][] y;
	
	
	
	public Otimizacao(Integer[] n, Integer[] m, Integer[] l, Integer[] v, Integer[] d, Integer[] a, String[] k,
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
			
			model.write("grb.lp");
			model.optimize();
		} catch (GRBException exc) {
			exc.printStackTrace();
		}
	
		
	}
	
	private void criaVariaveis(GRBModel model) {
		
		x = new GRBVar[n[n.length-1]+1][m[m.length-1]+1];
		u = new GRBVar[n[n.length-1]+1][l[l.length-1]+1];
		y = new GRBVar[n[n.length-1]+1][m[m.length-1]+1][l[l.length-1]+1];
		
		try {
			
			for(int i : n) {
				for(int j : m) {
					x[i][j] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, "xI"+Integer.toString(i)+"J"+Integer.toString(j));
				}
			}
			
			for(int i : n) {
				for(int j : l) {
					u[i][j] = model.addVar(0.0, 1.0, 0.0, GRB.BINARY, "uI"+Integer.toString(i)+"L"+Integer.toString(j));
				}
			}
			
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
	
	private void criaFO(GRBModel model) throws GRBException {
		
		GRBLinExpr expr = new GRBLinExpr();

		for(int i : n) {
			for(int j=a[i]; j<=m[m.length-1]; j++) {
				for(int w : l) {
					Integer cons = (int) Math.ceil((j-a[i] +1)/h[i][w]);
					expr.addTerm(cons, y[i][j][w]);
				}
			}
		}
	
		model.setObjective(expr, GRB.MINIMIZE);
		
	}
	
	private void criaConstrants(GRBModel model) throws GRBException {
		
		GRBLinExpr expr;

		//Constraint 4.23
		for(int i : n) {
			expr = new GRBLinExpr();
			for(int j=1; j<a[i]; j++) {
				expr.addTerm(1.0, x[i][j]);	
			}
			model.addConstr(expr, GRB.EQUAL, 0.0, "4.23I"+Integer.toString(i));
		}
		
		//Constraint 4.24
		for(int i : n) {
			expr = new GRBLinExpr();
			for(int j=a[i]; j<=m[m.length-1]; j++) {
				expr.addTerm(1.0, x[i][j]);
			}
			GRBLinExpr expr2 = new GRBLinExpr();
			for(int w : l) {
				expr2.addTerm(h[i][w], u[i][w]);
			}
			model.addConstr(expr, GRB.EQUAL, expr2, "4.24N"+Integer.toString(i));
		}
		
		//Constraint 4.25
		for(int j : m) {
			for(int w : l) {
				expr = new GRBLinExpr();
				for(int i : n) {
					expr.addTerm(1.0, y[i][j][w]);
				}
				model.addConstr(expr, GRB.LESS_EQUAL, 1.0, "4.25M"+Integer.toString(j)+"L"+Integer.toString(w));
			}
		}
		
		//Constraint 4.26
		for(int i : n) {
			expr = new GRBLinExpr();
			for(int j : l) {
				expr.addTerm(1.0, u[i][j]);
			}
			model.addConstr(expr, GRB.EQUAL, 1.0, "4.26N"+Integer.toString(i));
		}
		
		//Constraint 4.27
		for(int i : n) {
			for(int j=a[i]+1; j<=m[m.length-1]; j++) {
				expr = new GRBLinExpr();
				for(int z=1; z<=j-1; z++) {
					expr.addTerm(1.0, x[i][z]);
					expr.addTerm(-1.0*j, x[i][j-1]);
					expr.addTerm(j, x[i][j]);
					
					model.addConstr(expr, GRB.LESS_EQUAL, j, "4.27N"+Integer.toString(i)+"M"+Integer.toString(j));
				}
			}
		}
		
		//Constraint 4.28
		for(int j : m) {
			for(String  w : k) {
				expr = new GRBLinExpr();
				
				for(int i : n) {
					for(int z=1; z<=j; z++) {
						for(int b : l) {
							expr.addTerm(q[i].get(w)/h[i][b], y[i][z][b]);
						}
					}
				}
				
				model.addConstr(expr, GRB.GREATER_EQUAL, j*c.get(w)-e.get(w), "4.28M"+Integer.toString(j)+"K"+w);
				
			}
		}
		
		//Constraint 4.29-4.30-4.31
		for(int i : n) {
			for(int j : m) {
				for(int w : l) {
					String name = "N"+Integer.toString(i)+"M"+Integer.toString(j)+"L"+Integer.toString(w);
					//4.29
					expr = new GRBLinExpr();
					expr.addTerm(1.0, x[i][j]);
					expr.addTerm(1.0, u[i][w]);
					expr.addConstant(-1.0);
					
					model.addConstr(y[i][j][w], GRB.GREATER_EQUAL, expr, "4.29"+name);
					
					//4.30
					model.addConstr(y[i][j][w], GRB.LESS_EQUAL, x[i][j], "4.30"+name);
					
					//4.31
					model.addConstr(y[i][j][w], GRB.LESS_EQUAL, u[i][w], "4.31"+name);
				}
			}
		}
	}
}
