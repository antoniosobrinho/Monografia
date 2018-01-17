package auxilio;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Scanner;

import gurobi.GRB;
import gurobi.GRBLinExpr;
import leitura.LerInstancia;

public class Valida {

	private static Integer[] n;
	private static Integer[] m;
	private static String[] k;
	private static Integer[] l;
	private static Integer[] v;
	private static Integer[] d;
	private static Integer[] a;
	private static Double[][] h;

	
	private static HashMap<String, Double> e;
	private static HashMap<String, Double> c;

	private static HashMap<String, Double>[] q;
	
	public static void main(String args[]) {
		
			
		try {
			Scanner inputM = new Scanner(new FileReader("resultadoM.txt"));
			Scanner inputB = new Scanner(new FileReader("resultadoB.txt"));

			
			LerInstancia instancia = new LerInstancia("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/instancia.demur.50.250.3A.txt");

			n = instancia.getN();
			m = instancia.getM();
			k = instancia.getK();
			l = instancia.getL();
			v = instancia.getV();
			d = instancia.getD();
			a = instancia.getA();
			e = instancia.getE();
			c = instancia.getC();
			q = instancia.getQ();
			
			h = new Double[n[n.length-1]+1][l[l.length-1]+1];
			
			for(int i : n) {
				for(int j : l) {
					int soma=0;
					for(String p : k) {
						soma+=q[i].get(p);
					}
					h[i][j] = Math.ceil((double) soma/v[j]);
					
				}
			}
			
			Integer[] ib = new Integer[n.length+1];
			Integer[] im = new Integer[n.length+1];
			
			for(int i : n) {
				ib[i] = inputB.nextInt();
				im[i] = inputM.nextInt();
			}
			
			Integer[][][] y = new Integer[n.length+1][m.length+1][l.length+1];
			
			for(int i : n) {
				for(int j : m) {
					for(int b : l) {
						y[i][j][b] =(ib[i]==b && im[i]==j)? 1 : 0;
					}
				}
			}
			
			boolean res = true;
			int soma;
			
			for(int i : n) {
				soma = 0;
				for(int j=1; j<a[i]-1; j++) {
					for(int b : l) {
						soma += y[i][j][b];
					}
				}
				if(soma != 0) res = false;
			}
			
			if(res) System.out.println("Restrição 1 ok");
			
			res = true;
			
			for(int i : n) {
				soma = 0;
				for(int j=a[i]; j<m[m.length-1]; j++) {
					for(int b : l) {
						soma += y[i][j][b];
					}
				}
				if(soma != 1) res = false;
			}
			
			if(res) System.out.println("Restrição 2 ok");
			
			
			res = true;
			
			for(int i : n) {
				for(int j : m) {
					for(int b : l) {
						
						soma = 0;
						
						for(int i2 : n) {
							if(i2!=i) {
								for(int k=j; k<=j+h[i][b]-1; k++) {
									if(k>m[m.length-1]) 
										continue;
									soma += y[i2][k][b];							
								}
							}
						}
						
						if(soma > (1 - y[i][j][b])*n[n.length-1]) res = false;
					}
				}
			}
			
			if(res) System.out.println("Restrição 3 ok");
			
			res = true;
			
			for(int j : m) {
				for(String p : k) {
					soma = 0;
					
					for(int i : n) {
						for(int b : l) {
							for(int z=a[i]; z<=j; z++) {
								soma += ((Math.min(j-a[i]+1, h[i][b])/h[i][b])*q[i].get(p)) * y[i][z][b];
							}
						}
					}
					if(soma < j*c.get(p) - e.get(p)) res = false;
				}	
			}
			
			if(res) System.out.println("Restrição 4 ok");
			
			
			soma = 0;
			for(int i : n) {
				for(int j : m) {
					for(int b : l) {
						soma += (j +h[i][b] - a[i])*y[i][j][b];
					}
				}
			}	
			
			System.out.println(soma);
						
		} catch (FileNotFoundException ex) {
			// TODO Auto-generated catch block
			ex.printStackTrace();
		}
			
			
	
	}
}
