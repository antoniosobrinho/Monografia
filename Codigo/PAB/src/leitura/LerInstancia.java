package leitura;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

public class LerInstancia {

	private ArrayList<Integer> n = new ArrayList<Integer>();
	private ArrayList<Integer> m = new ArrayList<Integer>();
	private ArrayList<String> k = new ArrayList<String>();
	private ArrayList<Integer> l = new ArrayList<Integer>();
	
	private Integer[] v;
	private Integer[] d;
	private Integer[] a;
	
	private HashMap<String, Double> e = new HashMap<String,Double>();
	private HashMap<String, Double> c = new HashMap<String,Double>();

	private  HashMap<String, Double>[] q;
	
	public LerInstancia(String arquivo) {
		
		try {
			Scanner input = new Scanner(new FileReader(arquivo));
			
			
			//Lẽ o conjunto N
			input.next();
			int quant= input.nextInt();
			for(int i=1; i<=quant; i++) {
				n.add(i);
			}
			
			//Lê o conjunto M
			input.next();
			quant = input.nextInt();
			for(int i=1; i<=quant; i++) {
				m.add(i);
			}
			
			//Lê o conjunto K
			input.next();
			String aux = input.next();
			while(!aux.equals(";")) {
				k.add(aux);
				aux = input.next();
			}
		
			//Lê o conjunto L
			input.next();
			quant = input.nextInt();
			for(int i=1; i<=quant; i++) {
				l.add(i);
			}
			
			//Lê o conjunto V
			input.next();
			v = new Integer[l.size()+1];
			for(int i : l) {
				input.nextInt();
				v[i] = input.nextInt();
			}
		
			//Lê o conjunto D
			input.next();
			d = new Integer[n.size()];
			for(int i=0; i<n.size(); i++) {
				input.nextInt();
				d[i] = input.nextInt();
			}
			
			//Lẽ o conjunto A
			input.next();
			a = new Integer[n.size()+1];
			for(int i : n) {
				input.nextInt();
				a[i] = input.nextInt();
			}
			
			//Lê o conjunto E
			input.next();
			for(int i=0; i<k.size(); i++) {
				e.put(input.next(), input.nextDouble());
			}
			
			//Lê o conjunto C
			input.next();
			for(int i=0; i<k.size(); i++) {
				c.put(input.next(), input.nextDouble());
			}
			
			//Lê o conjunto Q
			input.next();
			String[] prod = new String[k.size()];
			for(int i=0; i<k.size(); i++) {
				prod[i] = input.next();
			}
			
			q = (HashMap<String, Double>[])new HashMap[n.size()+1];
			
			for(int i : n) {
				input.nextInt();
				q[i] = new HashMap<String, Double>();
				for(int j=0; j<prod.length; j++) {
					q[i].put(prod[j], input.nextDouble());
				}
			}
			input.close();
		
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public Integer[] getN() {
		return n.toArray(new Integer[n.size()]);
	}
	
	public Integer[] getM() {
		return m.toArray(new Integer[m.size()]);
	}
	
	public String[] getK() {
		return k.toArray(new String[k.size()]);
	}
	
	public Integer[] getL() {
		return l.toArray(new Integer[l.size()]);
	}
	
	public Integer[] getV(){
		return v;
	}
	
	public Integer[] getD() {
		return d;
	}
	
	public Integer[] getA() {
		return a;
	}
	
	public HashMap<String, Double> getE(){
		return e;
	}
	
	public HashMap<String, Double> getC(){
		return c;
	}
	
	public HashMap<String, Double>[] getQ(){
		return q;
	}
}
