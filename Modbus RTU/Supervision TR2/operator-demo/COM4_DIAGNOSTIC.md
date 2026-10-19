# COM4 muet après l’audit — diagnostic du 19/10/2026

Mission `TR2-20261019-LIVE-COM4-LOSS-DIAG`, firmware et paquet physique c36c971 inchangés. Le bilan précédent reste une preuve des cycles bornés, mais son affirmation de disponibilité **après l’audit GDB final** est corrigée ici : aucune vérification B0 après cet audit n’avait été effectuée. Ne pas réécrire les archives historiques.

## Cause observée et reproduite

Le STM32 est dans la boucle terminale HAL_Delay, r4u_result.error6, engine LIVE_FAULT, work.deadline_missed=true et service846318µs pour une limite2500µs. FIFO arrêtée, dernier STOP5/17 complet, journal94/root107 et FRAMe18e…ec15 inchangés. IRQ actives ; la file RX contient les requêtes B0 que la boucle applicative arrêtée ne consomme plus. Les timeouts PC ne proviennent donc pas d’un START en cours ou d’un refus Windows. Le service utilisateur conserve276 CommunicationLost/260émissions B0 sans réception ; les traces opérateur de14:19 sont rapportées par l’utilisateur et le contrat, pas inventées comme fichier local.

L’ancien audit GDB invoquait `stm32_fram_storage_read(...,262144)` à l’adresse d’arrêt aléatoire, alors qu’un callback chronométré était interrompu. Ce READ prend environ846ms sur SPI2,5MHz. À la continuation, ce temps supplémentaire est inclus dans la mesure du callback ; le garde-fou2,5ms arrête correctement l’application. Il n’a jamais été conçu pour ignorer une fonction injectée par le débogueur.

Une reproduction ciblée, FIFO inactive et sans commande B5/écriture persistante, confirme service846632µs et le même défaut6. FRAM intégrale exacte avant/après. Un reset logiciel encadré rétablit B0/B3, sans flash ni restauration FRAM. Deux resets nécessaires au diagnostic : remise en état pour reproduction, puis sortie du défaut reproduit.

## Procédure corrigée

Ne pas injecter de fonction cible lors d’un arrêt GDB quelconque, même si le capteur est arrêté. Ne pas effacer les compteurs, désactiver les deadlines ou modifier `work.servicing/seen/error` pour rendre la lecture possible.

`tools/capture_stopped_fram.py` est limité à l’ELF/BIN c36c971 qualifié et au TR2-001. Son exécution reste soumise aux permissions de la mission et aux règles AGENTS.md. Il exige :

1. STOP natif terminal4/0 avec effet arrêté/HELD et preuve JSONL conservée.
2. Service Windows fermé proprement ; aucun autre observateur/débogueur. Refus avant connexion cible si un tel processus existe.
3. Identité, image Flash exacte, absence de FIFO/transaction active, dernier STOP courant correspondant à la preuve et absence de faute.
4. Breakpoint à l’entrée du prochain `live_runtime_step`, hors scope : depth0, work.servicing=false, runtime.main_busy=false et runtime.servicing=false. **Seulement alors** appel READ FRAM complet, sans écriture persistante/reset/flash.
5. Détachement, redémarrage normal de l’observation et **preuve B0/B3 réellement reçue après l’audit**, sans reset interposé. Une capture de FRAM réussie seule ne prouve pas la disponibilité du firmware à la continuation.

```bash
python3 tools/capture_stopped_fram.py \
  --elf /chemin/vers/tr2-live-operator-NON-PRODUCTION.elf \
  --evidence /home/lolo/dev/msm/reports/NOUVEL_AUDIT \
  --stop-proof /chemin/vers/STOP-terminal.jsonl
```

Le chemin de preuve doit être neuf ; les exemples sont à remplacer par des artefacts vérifiés, pas une invitation à utiliser un ancien couple epoch/transaction. Le script ne modifie aucune garde temporelle ni version Windows. Un service actif a été effectivement refusé avant toute connexion GDB ; une lecture à la frontière corrigée a conservé FRAM et disponibilité B0/B3.

La remise en service START/STOP et l’état final sont consignés dans [DERNIER_RAPPORT.md](../../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md). Le nouveau correctif de pause Windows reste non déployé et le chantier de signature reste séparé. La durée prolongée11min50s antérieure n’est pas rejouée pour ce diagnostic d’outillage.
