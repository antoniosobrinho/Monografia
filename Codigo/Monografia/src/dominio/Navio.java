package dominio;

import java.util.HashMap;

public class Navio {

	private HashMap<Integer, Integer> produtosQuantidade;
	
	private int pier, id;

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public HashMap<Integer, Integer> getProdutosQuantidade() {
		return produtosQuantidade;
	}

	public void setProdutosQuantidade(HashMap<Integer, Integer> produtosQuantidade) {
		this.produtosQuantidade = produtosQuantidade;
	}

	public int getPier() {
		return pier;
	}

	public void setPier(int pier) {
		this.pier = pier;
	}
}
