# MSM TR2 — donnée physique complémentaire pour V1.7 : bouton USER

Date : 2026-11-04. Observation communiquée par l'utilisateur, mesurée sur Nucleo TR2-001.

- Hauteur du sommet du bouton bleu USER **au repos** au-dessus de la face supérieure du PCB Nucleo : **10,2 mm** (mesure déjà communiquée).
- Course observée jusqu'au **premier déclenchement / clic** de USER : **environ 0,4 mm** (nouvelle mesure utilisateur).
- Hauteur indicative correspondante du sommet du bouton au déclenchement : **9,8 mm** au-dessus du PCB, par soustraction 10,2 - 0,4, sous réserve d'incertitude de mesure et de mêmes surfaces de référence.

Cette valeur est une **course de déclenchement observée**, **pas** une course totale admissible, une butée mécanique certifiée, ni une donnée sur la force ou la tolérance du poussoir.

**Consigne d'intégration :** utiliser cette donnée pour le dimensionnement de principe du poussoir extérieur captif : jeu réel au repos, butée indépendante reprenant les efforts sur le couvercle/structure, pointe souple légèrement compliante, absence d'appui permanent, prise en compte des tolérances et de la course réelle du bouton. Ne jamais régler une butée rigide à exactement 0,4 mm sans garde de tolérance : vérifier la référence constructeur et prévoir l'ajustabilité. RESET reste protégé et sans accès d'exploitation.

Le présent complément **n'altère pas** `MISSION_EN_COURS.md`, le travail en cours de Codex, la CAO PCB ni les rapports historiques ; il peut être exploité lors de la tranche V1.7 ou intégré à sa revue ultérieure. Pas d'autorisation de flash/debug, de manipulation STM32 ni d'achat.
