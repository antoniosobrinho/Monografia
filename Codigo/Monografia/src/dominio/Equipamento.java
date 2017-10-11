package dominio;

import java.util.ArrayList;

public class Equipamento {

	private ArrayList<Integer> rotas;
	private int capacidadeTransporte;
	
	public ArrayList<Integer> getRotas() {
		return rotas;
	}
	public void setRotas(ArrayList<Integer> rotas) {
		this.rotas = rotas;
	}
	public int getCapacidadeTransporte() {
		return capacidadeTransporte;
	}
	public void setCapacidadeTransporte(int capacidadeTransporte) {
		this.capacidadeTransporte = capacidadeTransporte;
	}
	
}
