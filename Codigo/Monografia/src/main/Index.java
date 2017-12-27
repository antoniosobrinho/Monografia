package main;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;
import java.util.Set;

import dominio.Equipamento;
import dominio.Navio;
import dominio.Periodo;
import dominio.Rota;
import dominio.RotasConflito;
import dominio.SubArea;
import leitura.LerInstancias;

public class Index {

	public static void main(String[] args) {
			
		Scanner input = new Scanner(System.in);
		
		String diretorio = "/home/mourao/Documentos/Computação/Monografia/InstanciasArtigo_Tipo1/Instancia04Periodos/5Prod50Maq90Rota20Areas";
		
		input.close();

		LerInstancias lerInstancias = new LerInstancias(diretorio);

		SubArea[] subAreas = new SubArea[lerInstancias.getNumSubAreas()];
		Equipamento[] equipamentos = new Equipamento[lerInstancias.getNumEquipamentos()];
		Periodo[] periodos = new Periodo[lerInstancias.getNumPeriodos()];
		
		ArrayList<Integer>[] produtosSubareas = lerInstancias.getProdutosSubarea();
		Integer[][] estoqueSubareas = lerInstancias.getEstoqueSubarea();
		Integer[] capacidadeTransporte = lerInstancias.getCapacidadeEquipamentos();
		ArrayList<Integer>[] rotasEquipamentos = lerInstancias.getRotasEquipamentos();
		HashMap<Integer,Integer>[] chegadaProdutos = lerInstancias.getChegadaProdutos();
		ArrayList<Navio>[] naviosPeriodo = lerInstancias.getNaviosPeriodo();
		Integer[] produtos = lerInstancias.getProdutos();
		Integer[] piers = lerInstancias.getPiers();
		
		//Seta as rotas
		Rota[] rotas = lerInstancias.getRotas();
		for(int i=0; i<rotas.length; i++) {
			rotas[i].setEquipamentos(rotasEquipamentos[i]);
		}
		//seta todas as subAreas com suas caracteristicas
		for(int i=0; i<subAreas.length;i++) {
			subAreas[i] = new SubArea();
			subAreas[i].setProdutosSuportados(produtosSubareas[i]);
			subAreas[i].setProdutoEstocado(estoqueSubareas[i][0]);
			subAreas[i].setQuantidadeEstocada(estoqueSubareas[i][1]);
			subAreas[i].setCapacidade(estoqueSubareas[i][2]);
		}
				
		//seta todos os equipamentos com suas caracteristicas
		for(int i=0; i<equipamentos.length;i++) {
			equipamentos[i] = new Equipamento();
			equipamentos[i].setCapacidadeTransporte(capacidadeTransporte[i]);
		}
		
		//seta todos os periodos com suas caracteristicas
		for(int i=0; i<periodos.length; i++) {
			periodos[i] = new Periodo();
			periodos[i].setChegadaProdutos(chegadaProdutos[i]);
			periodos[i].setNavios(naviosPeriodo[i].toArray(new Navio[naviosPeriodo[i].size()]));
		}
		
		ArrayList<RotasConflito> rc = new ArrayList<RotasConflito>();
		for(int r=0; r<rotas.length-1; r++) {
			for(int rl=r+1; rl<rotas.length;rl++) {
				for(int e=0; e<equipamentos.length; e++) {
					if(rotas[r].getEquipamentos().contains(e)&&rotas[rl].getEquipamentos().contains(e)) {
						rc.add(new RotasConflito(rotas[r], rotas[rl]));
						break;
					}
				}
			}
		}
		
		Otimizacao otimo = new Otimizacao(subAreas, equipamentos, periodos, rotas, produtos, piers, rc.toArray(new RotasConflito[rc.size()]));
				
	}
	
	
}
