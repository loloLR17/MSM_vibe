---
mission_id: TR2-20261013-M016-SDMMC-1BIT-LOAD-ONLY
status: READY
base_ref: main
base_sha: 3538017a361d7178a1ac19a9563aeaf2eae1af37
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M016 — Un seul essai de charge SDMMC2 1 bit, répétable

## Priorité absolue — décision utilisateur
Quota restant faible : **mission réduite à un seul scénario de charge pertinent TR2**, exécutable plusieurs fois à l'identique pour reproductibilité. Pas de diagnostic 4 bits, pas de recherche exploratoire, pas de matrice de tests, pas de refonte ni d'optimisation prématurée. L'utilisateur demande efficacité et résultat exploitable dès ce soir. **MicroSD SanDisk Ultra 32 Go de test déjà insérée, seconde Nucleo connectée sur ST-LINK**. Le média de test est sacrifiable. Aucun oscilloscope disponible : ne pas en demander.

## Références
Lire HEAD main, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, dernier rapport GPIO, rapport SDMMC-REFERENCE-PHYSICAL, README du démonstrateur `experiments/sdmmc_st_reference`, H3h-C2c, P8-B et arbitrage H3h-A. Ne pas modifier les spécifications gelées. Référence de débit : 26667 échantillons/s × 16 octets = **426672 B/s**. Le précédent C2c à 1 bit / 10 MHz avait validé 512 KiB à 1,20 MB/s moyen ; une latence isolée de 298 ms/64 KiB avait été observée.

## Sécurité et transition depuis GPIO — NON NÉGOCIABLE
Le dernier rapport indique que le firmware GPIO actuel configure PB15/PB3/PB4 en **sorties push-pull basses**. L'utilisateur vient d'indiquer que **la microSD est désormais insérée et la Nucleo sous ST-LINK**. Cette configuration peut provoquer une contention électrique avec une carte SD alimentée. **Avant toute commande GPIO, reset expérimental, test SDMMC ou flash**, identifier la seconde sonde ST-LINK 0032001F3235510C37333439 / UID 001B0046/38425018/20333157, puis faire immédiatement mettre le banc hors tension par l'utilisateur et demander de retirer la microSD. Ne pas supposer que la situation est sans danger ni faire tourner l'ancien firmware. Préparer et flasher ensuite une image SD sûre **avec carte retirée**, qui n'active pas les sorties GPIO basses et configure le bus SDMMC1bit correctement ; vérifier la configuration. Demander ensuite insertion microSD **hors tension**, puis reconnexion ST-LINK. Si l'utilisateur confirme qu'un autre firmware SD sûr a déjà remplacé le firmware GPIO, vérifier preuves réelles avant de sauter la séquence. Jamais de flash principal.

## Un seul test physique à exécuter
1. Réutiliser le démonstrateur indépendant dérivé de l'exemple ST CubeU5 v1.9.0 `SD_ReadWrite_DMALinkedList` déjà présent, adaptations minimales et documentées, HAL officiel inchangé. **Mode 1 bit forcé et vérifié sur carte et CLKCR**, fréquence cible **10 MHz** déjà caractérisée, fréquence effective prouvée.
2. Définir **une campagne continue unique de 10 minutes**, simulant une arrivée logique à **426672 B/s**, soit environ **256 MB** en 600 s, avec écriture brute sur la microSD de test. Tampon de capacité fixe bornée, enregistrer backlog maximal, débordement, latences de chaque chunk (ou histogramme + P95/P99/max), débit utile, HAL/SDMMC errors. Choisir taille chunk raisonnable documentée (64 KiB par défaut) et zone LBA sûre sur carte sacrifiable après lecture CID/capacité. Aucune répétition automatique ; campagne déclenchable manuellement à nouveau à paramètres identiques, pour comparaison ultérieure.
3. **Intégrité impérative** : données déterministes vérifiables ; relecture intégrale de la zone écrite après campagne et comparaison fiable (octet par octet ou hash par chunk avec comptage des erreurs). Si ressources RAM limitées, vérifier en flux. Reporter nombre d'octets écrits/relus et première divergence. Ne pas masquer les échecs par retries silencieux.
4. Un seul scénario, sans benchmark annexe. Codex peut réaliser les préparations logicielles et un court contrôle d'initialisation strictement nécessaire pour garantir que la campagne n'endommagera pas le banc, mais pas d'essais supplémentaires exploratoires. **Exécuter une campagne complète si techniquement possible** ; si blocage, fournir la première cause prouvée et le correctif minimal justifié, sans enchaîner des changements spéculatifs.
5. Fournir résultat explicite : durée effective, octets logiques générés, écrits, vérifiés, débit moyen, latence max/P95/P99, backlog maximal, pertes, erreurs, CID, UID, clock/bus réels, empreintes et code exact. Séparer succès du banc de qualification du runtime TR2. **Ne pas qualifier 7 jours sur 10 minutes**.

## Permissions et sortie
`allow_commit: true`, `allow_push: true`, `allow_flash: true`, `allow_debug: true`. Autonomie Codex maximale après gestes physiques, sans validations répétitives. Uniquement seconde Nucleo et microSD de test, jamais principal, FRAM, Modbus, autres matériels. Pas de mass erase/OB/RDP, pas de formatage inutile. Publier code, preuves et `DERNIER_RAPPORT.md` sur main et vérifier distant. L'utilisateur relancera plus tard exactement le même test pour reproductibilité. ChatGPT contrôlera le rapport indépendamment.
