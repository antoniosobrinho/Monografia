package dominio;

public class Rota {

	public static final int RECEPCAO_PATIO = 1;
	public static final int RECEPCAO_PIER = 2;
	public static final int PATIO_PIER = 3;
	
	private int tipo, capacidade, distancia, pier, subArea;
	
	public Rota(int tipo) {
		setTipo(tipo);
		switch(tipo) {
			case(RECEPCAO_PATIO):
				setPier(-1);
				break;
			case(RECEPCAO_PIER):
				setSubArea(-1);
				break;
			default:
				break;
		}
		
	}

	public int getPier() {
		return pier;
	}

	public void setPier(int pier) {
		this.pier = pier;
	}

	public int getSubArea() {
		return subArea;
	}

	public void setSubArea(int subArea) {
		this.subArea = subArea;
	}

	private void setTipo(int tipo) {
		this.tipo = tipo;
	}
	
	public int getTipo() {
		return tipo;
	}
	
	public int getCapacidade() {
		return capacidade;
	}

	public void setCapacidade(int capacidade) {
		this.capacidade = capacidade;
	}

	public int getDistancia() {
		return distancia;
	}

	public void setDistancia(int distancia) {
		this.distancia = distancia;
	}
	
}
