package main;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

import dominio.Equipamento;
import dominio.Navio;
import dominio.Periodo;
import dominio.Rota;
import dominio.SubArea;
import leitura.LerInstancias;

public class Index {

	public static void main(String[] args) {
			
		Scanner input = new Scanner(System.in);
		
		String diretorio = input.next();
		
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
			equipamentos[i].setRotas(rotasEquipamentos[i]);
		}
		
		//seta todos os periodos com suas caracteristicas
		for(int i=0; i<periodos.length; i++) {
			periodos[i] = new Periodo();
			periodos[i].setChegadaProdutos(chegadaProdutos[i]);
			periodos[i].setNavios(naviosPeriodo[i].toArray(new Navio[naviosPeriodo[i].size()]));
		}
	
			
		Otimizacao otimo = new Otimizacao(subAreas, equipamentos, periodos, rotas, produtos, piers);
		
		System.out.println("terminou");
	}
}
