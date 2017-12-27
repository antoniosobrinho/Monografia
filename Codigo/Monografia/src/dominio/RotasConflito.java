package dominio;

public class RotasConflito {

	private Rota r1;
	private Rota r2;
	
	public RotasConflito(Rota r1, Rota r2) {
		this.r1 = r1;
		this.r2 = r2;
	}
	
	public void setRotas(Rota r1, Rota r2) {
		this.r1 = r1;
		this.r2 = r2;
	}
	
	public Rota getR1() {
		return this.r1;
	}
	
	public Rota getR2() {
		return this.r2;
	}
}
