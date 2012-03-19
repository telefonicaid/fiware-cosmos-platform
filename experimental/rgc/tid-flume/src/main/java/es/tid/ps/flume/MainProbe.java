package es.tid.ps.flume;

public class MainProbe {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		MainProbe mp = new MainProbe();
		mp.aaa();

	}
	
	public void aaa(){
		Padre padre = (Padre) new Hijo();
		padre.hola();
	}
	
	
	private class Padre{
		public void hola(){
			System.out.print("padre");
		}
	};
	
	private  class Hijo extends Padre{
		public void hola(){
			System.out.print("hijo");
		}
	}

}
