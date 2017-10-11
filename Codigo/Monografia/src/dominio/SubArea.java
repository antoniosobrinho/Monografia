package dominio;

import java.util.ArrayList;

public class SubArea {

	private ArrayList<Integer> produtosSuportados;
	private int capacidade, produtoEstocado, quantidadeEstocada;
	
	public int getCapacidade() {
		return capacidade;
	}

	public void setCapacidade(int capacidade) {
		this.capacidade = capacidade;
	}

	public int getProdutoEstocado() {
		return produtoEstocado;
	}

	public void setProdutoEstocado(int produtoEstocado) {
		this.produtoEstocado = produtoEstocado;
	}

	public int getQuantidadeEstocada() {
		return quantidadeEstocada;
	}

	public void setQuantidadeEstocada(int quantidadeEstocada) {
		this.quantidadeEstocada = quantidadeEstocada;
	}

	public void setProdutosSuportados(ArrayList<Integer> produtorsSuportados) {
		this.produtosSuportados = produtorsSuportados;
	}
	
	public ArrayList<Integer> getProdutosSuportados(){
		return produtosSuportados;
	}
}
