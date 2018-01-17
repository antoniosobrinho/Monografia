package auxilio;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Random;

public class CriaInstancia {

	public static void main(String[] args) throws IOException {
		
		
	
		String[] letras = {"A", "B", "C", "D", "E"};
		/*for(int navios = 5; navios<=30; navios+=5) {
			for(int bercos=1; bercos<=6; bercos++) {
				for(int i=0; i<5;i++) {
					String nome = Integer.toString(navios)+"."+Integer.toString(bercos)+"."+letras[i]+".txt";
					criaInstancia(navios, bercos, nome);
				}
			}
		}
		*/

		criaInstancia(50, 2, "50.2.txt");

	}
	
	public static void criaInstancia(int navios, int bercos, String nome) throws IOException {
		
		Random rand = new Random();
		
		Integer[] velBercos = {8, 16, 24, 8, 16, 24};
		String[] produtos = {"Alumina", "Bauxita", "Manganes", "Ferro"};
		
		PrintWriter instancia = new PrintWriter(new FileWriter(new File("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/NovasInstancia/"+nome)));
	
		instancia.write("N " + Integer.toString(navios) +"\n");
		instancia.write("M 250\n");
		instancia.write("K Alumina Bauxita Manganes Ferro ;\n");
		instancia.write("L "+Integer.toString(bercos)+"\n");
		instancia.write("v\n");
		for(int i=1; i<=bercos; i++) {
			instancia.write(Integer.toString(i)+" "+Integer.toString(velBercos[i-1])+"\n");
		}
		
		instancia.write("a\n");
		int a = rand.nextInt(1)+1;
		instancia.write("1 " + Integer.toString(a) + "\n");
		
		for(int i=2; i<=navios; i++) {
			a += rand.nextInt(3);
			instancia.write(Integer.toString(i) + " " + Integer.toString(a)+"\n");
		}
		
		instancia.write("e\n");
		
		for(int i=0; i<4; i++) {
			Double e = 15 + 15*rand.nextDouble();
			String aux = Double.toString(e).replace(".", ",");
			instancia.write(produtos[i] + " " + aux +"\n");
		}
		
		instancia.write("ck\n");
		
		for(int i=0; i<4; i++) {
			Double ck = 3 + 7*rand.nextDouble();
			String aux = Double.toString(ck).replace(".", ",");
			instancia.write(produtos[i] + " " + aux +"\n");
		}
		
		instancia.write("q Bauxita Alumina Manganes Ferro\n");
		
		for(int i=1; i<=navios; i++) {
			int k = rand.nextInt(4);
			double q = 4 + 16 * rand.nextDouble(); 
			String aux = Double.toString(q).replace(".", ",");
			instancia.write(Integer.toString(i));
			for(int j=0; j<4; j++) {
				if(j == k) {
					instancia.write(" "+aux);
				}
				else {
					instancia.write("  0");
				}
			}
			instancia.write("\n");
		}
		instancia.close();
	}
}
