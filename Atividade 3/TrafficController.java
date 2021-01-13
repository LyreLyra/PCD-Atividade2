import java.util.concurrent.Semaphore;

public class TrafficController {

    // Para desabilitar o sistema de "fairness", basta trocar true por false na linha abaixo.
    Semaphore semaforoPonte = new Semaphore(1, true);

    public void enterLeft() { try { semaforoPonte.acquire(); } catch (InterruptedException e) { System.out.println("int left"); } }
    public void enterRight() { try { semaforoPonte.acquire(); } catch (InterruptedException e) { System.out.println("int right"); } }
    public void leaveLeft() { semaforoPonte.release(); }
    public void leaveRight() { semaforoPonte.release(); }

}