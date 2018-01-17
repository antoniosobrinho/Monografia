package main;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;

import leitura.LerInstancia;

public class Index {

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
	
	
	public static void main(String[] args) throws IOException {
	/*
		PrintWriter tempos;
		String[] letras = {"A", "B", "C", "D", "E"};

		for(int i=5; i<=30; i+=5) {
			tempos = new PrintWriter(new FileWriter(new File("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/NovasInstancia/TempoNovasInstancias/tempos"+Integer.toString(i)+".txt")));
			for(int j=1; j<=6; j++) {
				tempos.write(Integer.toString(i)+"."+Integer.toString(j));
				for(String letra : letras) {
					String instancia = Integer.toString(i)+"."+Integer.toString(j)+"."+letra+".txt";
					double time = otimiza(instancia);
					tempos.write(" "+time/1000);
				}
				tempos.write("\n");
			}
			tempos.close();
		}*/
		
		otimiza("50.2.txt");
	
	}
	
	public static double otimiza(String nome) {
		LerInstancia instancia = new LerInstancia("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/NovasInstancia/"+nome);

		n = instancia.getN();
		m = instancia.getM();
		k = instancia.getK();
		l = instancia.getL();
		v = instancia.getV();
		//d = instancia.getD();
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
		
		//Otimizacao otimizacao = new Otimizacao(n, m, l, v, d, a, k, e, c, q, h);
		
		PGME2 pgme2 = new PGME2();
		
		pgme2.Otimizacao(n, m, l, v, d, a, k, e, c, q, h);
		
		return pgme2.getTime();
	}
}
