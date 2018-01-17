package auxilio;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Scanner;

public class Media {

	public static void main(String[] args) throws IOException {
		Scanner arq;
		
		FileWriter file = new FileWriter(new File("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/NovasInstancia/TempoNovasInstancias/medias"));
		for(int i=5; i<=30; i+=5) {
			arq = new Scanner(new FileReader("/home/mourao/Documentos/Computação/Monografia/InstanciasPAB/NovasInstancia/TempoNovasInstancias/tempos"+Integer.toString(i)+".txt"));
			double soma = 0;
			
			arq.next();
			
			for(int k=1; k<=6;k++) {
				for(int j=0; j<5;j++) {
					soma += Double.parseDouble(arq.next());
				}
				
				soma = soma/5;
				
				file.write(Integer.toString(i)+"."+Integer.toString(k)+" "+Double.toString(soma).replace(".", ",")+"\n");
			}
			
		}
	
		file.close();
		
	}
}
