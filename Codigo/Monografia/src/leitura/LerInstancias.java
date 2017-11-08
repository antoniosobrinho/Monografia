package leitura;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Scanner;

import dominio.Navio;
import dominio.Rota;

public class LerInstancias {

	private String diretorio;
	private Integer[][] estoquesSubarea;
	private	ArrayList<Integer>[] produtosSubarea,  rotasEquipamentos;
	private Integer[] capacidadeEquipamentos;
	private ArrayList<Navio>[] naviosPeriodo;
	private HashMap<Integer,Integer>[] chegadaProdutos;
	private int numSubareas, numEquipamentos, numPeriodos, numRotas;
	private Rota[] rotas;
	private Integer[] piers;
	private ArrayList<Integer> produtos = new ArrayList<Integer>();
	
	public LerInstancias(String diretorio) {
		this.diretorio = diretorio;
		
		lerDetalhes();
		lerProdutosSubarea();
		lerEstoqueSubArea();
		lerEquipamentoCapacidade();
		lerRotasEquipamento();
		lerChegadaProdutos();
		lerChegadaNavios();
		lerRotas();
		
	}
	
	private void lerDetalhes() {
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/detalhes.txt"));
			
			numSubareas = lerArq.nextInt();
			numEquipamentos = lerArq.nextInt();
			numPeriodos= lerArq.nextInt();
			numRotas = lerArq.nextInt();
			
			lerArq.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void lerProdutosSubarea(){
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/ProdutosSubarea.txt"));
			
			ArrayList<Integer>[] produtosSubarea = (ArrayList<Integer>[])new ArrayList[numSubareas];
			int area = 0;
			
			int produto;
			while(lerArq.hasNext()) {
				lerArq.nextInt();
				produtosSubarea[area] = new ArrayList<Integer>();
				while((produto = lerArq.nextInt())!=-1) {
					if(!produtos.contains(produto))
						produtos.add(produto);
					produtosSubarea[area].add(produto);
				}
				area++;
			}
			lerArq.close();
			this.produtosSubarea = produtosSubarea;
			
		}catch (IOException e) {
	        System.err.printf("Erro na abertura do arquivo: %s.\n",
	                e.getMessage());
	    }
			
	}
	
	private void lerEstoqueSubArea() {
		
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/ConfigSubArea.txt"));
			
			Integer[][] estoquesSubarea = new Integer[numSubareas][3];

			int area = 0;
			
			while(lerArq.hasNext()) {
				lerArq.nextInt();
				for(int i=0;i<3;i++) {
					estoquesSubarea[area][i] = lerArq.nextInt();			
				}
				area++;
			}
			lerArq.close();
			this.estoquesSubarea = estoquesSubarea;
		}catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void lerEquipamentoCapacidade() {
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/EquipCapacSubArea.txt"));
		
			Integer[] capacidadeEquipamentos = new Integer[numEquipamentos];

			int equipamento = 0;
			while(lerArq.hasNext()) {
				lerArq.nextInt();
				capacidadeEquipamentos[equipamento] = lerArq.nextInt();
				equipamento++;
			}
			lerArq.close();
			this.capacidadeEquipamentos = capacidadeEquipamentos;
		}catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void lerRotasEquipamento() {
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/MaqRota.txt"));
			
			ArrayList<Integer>[] rotasEquipamentos = (ArrayList<Integer>[])new ArrayList[numEquipamentos];			
			
			int equipamento = 0;
			int dado;
			while(lerArq.hasNext()) {
				lerArq.nextInt();
				rotasEquipamentos[equipamento] = new ArrayList<Integer>();
				while((dado = lerArq.nextInt())!=-1) {
					rotasEquipamentos[equipamento].add(dado);			
				}
				equipamento++;
			}
			lerArq.close();
			this.rotasEquipamentos = rotasEquipamentos;
		}catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void lerChegadaProdutos() {
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/ChegadaTrem.txt"));
			
			HashMap<Integer, Integer>[] chegadaProdutos = (HashMap<Integer, Integer>[]) new HashMap[numPeriodos];
			
			int produto, periodo;
			while(lerArq.hasNext()) {
				periodo = lerArq.nextInt()-1;
				chegadaProdutos[periodo] = new HashMap<Integer, Integer>();
				while((produto = lerArq.nextInt())!=-1) {
					chegadaProdutos[periodo].put(produto, lerArq.nextInt());
					if(!produtos.contains(produto)) produtos.add(produto);
				}
			}
			lerArq.close();
			this.chegadaProdutos = chegadaProdutos;
		}catch (IOException e) {
						e.printStackTrace();
		}
	}
	
	private void lerChegadaNavios() {
		try {
			Scanner lerArq = new Scanner(new FileReader(diretorio + "/ChegadaNavios.txt"));
				
			ArrayList<Navio>[] naviosPeriodo = (ArrayList<Navio>[])new ArrayList[numPeriodos];		
			HashMap<Integer,Integer> produtoQuantidade = new HashMap<Integer,Integer>();
			ArrayList<Integer> piers = new ArrayList<Integer>();
			int dado, periodo;
			Navio navio = new Navio();
			
			while(lerArq.hasNext()) {
				periodo = lerArq.nextInt();
				if(naviosPeriodo[periodo-1] == null) {
					naviosPeriodo[periodo-1] = new ArrayList<Navio>();
				}
				navio.setId(lerArq.nextInt());
				navio.setPier(lerArq.nextInt()-1);
				if(!piers.contains(navio.getPier()-1)){
					piers.add(navio.getPier()-1);
				}
				while((dado=lerArq.nextInt())!=-1) {
					produtoQuantidade.put(dado, lerArq.nextInt());
				}
				navio.setProdutosQuantidade(produtoQuantidade);
				naviosPeriodo[periodo-1].add(navio);
				navio = new Navio();
				produtoQuantidade = new HashMap<Integer,Integer>();
			}
			lerArq.close();
			this.naviosPeriodo = naviosPeriodo;
			this.piers = piers.toArray(new Integer[piers.size()]);
		}catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	
	private void lerRotas() {
		
		try {
			Scanner lerArq =  new Scanner(new FileReader(diretorio+"/RotaRX-RecePatio.txt"));
			Rota[] rotas = new Rota[numRotas];
			
			int rota;
			
			while(lerArq.hasNext()) {
				rota = lerArq.nextInt()-1;
				rotas[rota] = new Rota(Rota.RECEPCAO_PATIO);
				rotas[rota].setCapacidade(lerArq.nextInt());
				rotas[rota].setSubArea(lerArq.nextInt()-1);
				rotas[rota].setDistancia(lerArq.nextInt());
			}
			
			lerArq =  new Scanner(new FileReader(diretorio+"/RotaRY-RecePier.txt"));
			while(lerArq.hasNext()) {
				rota = lerArq.nextInt()-1;
				rotas[rota] = new Rota(Rota.RECEPCAO_PIER);
				rotas[rota].setCapacidade(lerArq.nextInt());
				lerArq.nextInt();
				rotas[rota].setPier(lerArq.nextInt()-1);
				rotas[rota].setDistancia(lerArq.nextInt());
			}
			
			lerArq =  new Scanner(new FileReader(diretorio+"/RotaRZ-PatioPier.txt"));
			while(lerArq.hasNext()) {
				rota = lerArq.nextInt()-1;
				rotas[rota] = new Rota(Rota.PATIO_PIER);
				rotas[rota].setCapacidade(lerArq.nextInt());
				rotas[rota].setSubArea(lerArq.nextInt()-1);
				rotas[rota].setPier(lerArq.nextInt()-1);
				rotas[rota].setDistancia(lerArq.nextInt());
			}
			
			this.rotas = rotas;
			lerArq.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	public int getNumSubAreas() {
		return numSubareas;
	}
	
	public int getNumEquipamentos() {
		return numEquipamentos;
	}
	
	public int getNumPeriodos() {
		return numPeriodos;
	}
	public ArrayList<Integer>[] getProdutosSubarea(){
		return produtosSubarea;
	}
	
	public Integer[][] getEstoqueSubarea(){
		return estoquesSubarea;
	}
	
	public Integer[] getCapacidadeEquipamentos(){
		return capacidadeEquipamentos;
	}
	
	public ArrayList<Integer>[] getRotasEquipamentos(){
		return rotasEquipamentos;
	} 
	
	public HashMap<Integer,Integer>[] getChegadaProdutos(){
		return chegadaProdutos;
	}
	
	public ArrayList<Navio>[] getNaviosPeriodo(){
		return naviosPeriodo;
	}
	
	public Rota[] getRotas(){
		return rotas;
	}
	
	public Integer[] getProdutos() {
		Integer[] produtos = this.produtos.toArray(new Integer[this.produtos.size()]);
		Arrays.sort(produtos);
		return produtos;
	}
	
	public Integer[] getPiers() {
		return piers;
	}
}
