---
mission_id: TR2-20261016-R4-COOPERATIVE-FRAM-FIFO-RT
status: READY
base_ref: main
base_sha: 66c5e167b31cbc7baa302843c354760569d1c88d
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4 — Ordonnancement coopératif FRAM/FIFO et qualification temps réel

## Contexte et références
R0 normatif, R1 FRAM/configuration, R2 moteur epoch/commandes, R3-A owner continu, R3-B métrologie/B3 sont livrés **au niveau logiciel**. Le binaire STM32 canonique reste V1 et ne publie pas protocole2. R3-B révèle un risque de blocage majeur : une phase R2 transfère 313916 octets lus + 52426 écrits ; à 2,5 MHz calculés, le seul temps utile de bus est >=1,172 s, alors que la FIFO 512 mots se remplit théoriquement en ~17 ms à la cadence visée. Aucune de ces durées n'est une mesure physique. Ne pas activer LIVE sur cette base.

Consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md §10, roadmap/état, contrat et normes R0, LIVE_R1/R2/R3A/R3B, missions et rapports, preuves et gouvernance actuelle sur main. Vérifier HEAD distant, archive complète de mission/rapport R3-B, préservation des gels V1/V1.1 et travaux locaux historiques. Respecter A-06.

## Objectif prioritaire
Résoudre **sans compromettre l'atomicité, la causalité ni la sûreté** la coexistence du service FIFO continu, du journal/configuration FRAM, des commandes START/STOP/RENEW/APPLY et du transport Modbus. Obtenir des marges chiffrées et vérifiables ; qualifier honnêtement les limites hôte/ARM/STM32. Ne pas engager le raccordement RTU2 tant que le service temps réel n'est pas démontré.

## Travaux
1. Cartographier la chaîne effective SPI1 FRAM, SPI3 IIS3DWB, interruptions, DMA éventuel, scheduler, CRC/SHA, transport Modbus et callbacks R1/R2. Mesurer/borner le pire coût de chaque transaction et phase, fréquence réelle, CS, retries, délais HAL et priorités. Vérifier l'hypothèse 2,5 MHz ; ne pas confondre bus SPI distincts et blocage CPU global.
2. Concevoir et implémenter une **transaction FRAM incrémentale coopérative** avec quanta bornés (32 octets point de départ à qualifier, non valeur imposée), yield FIFO entre quanta, CRC/hash et readback incrémentaux. Conserver exactement les formats et contrats R0, publication atomique, root/bank, barrières STARTED/effet/COMPLETED, récupération power-loss, idempotence et anti-rejeu ; aucune écriture physique si ownership FRAM LIVE non prouvé. Éviter toute réentrance dans les callbacks et toute inversion de priorité.
3. Donner la priorité au STOP sûr, indépendant de la réussite d'écriture FRAM, et au service FIFO ; prouver admission et réserves de ressources pour START/STOP/RENEW/APPLY. Si l'approche coopérative ne peut garantir les bornes, documenter l'alternative journal delta comme décision structurante **sans migrer silencieusement les formats**.
4. Instrumenter FIFO niveau maximal, gaps, overflow, temps par quantum, jitter, CPU/stack/RAM, latences Modbus et STOP ; scénarios cadence max, bursts, fautes SPI/FRAM, coupures entre chaque phase et sur les frontières de commit, service prolongé et absence de SD. Définir seuils d'acceptation issus des normes et capacités physiques réelles, pas des hypothèses arbitraires.
5. Raccorder le provider de temps d'échantillon R3-B uniquement avec une source calibrée et vérifiable ; si non disponible, conserver B3 invalide, ne jamais substituer un temps de lecture SPI. Ne pas annoncer protocole2 ni LIVE physique prématurément.
6. Réutiliser les composants existants et conserver V1 ; mettre à jour cartographie/LOC et supprimer uniquement des duplications démontrées. Réaliser tests hôte, injections, ASan/UBSan et cross-build ARM ; essais physiques T1000 uniquement après vérification des conditions matérielles.

## Matériel et autonomie
allow_flash=true, allow_debug=true : autorisations permanentes applicables sous AGENTS.md. T1000 seul banc ; identifier carte/UID/ST-LINK, image/état de flash, alimentation et câblage, vérifier sauvegardes et risques avant toute intervention. Aucune écriture FRAM LIVE [122880,245760) avant inventaire, preuve de propriété exclusive et sauvegarde vérifiée ; protéger zones historiques, Option Bytes/RDP et autres médias. Aucun mass erase, aucune intervention physique manuelle déléguée à Codex. Si les preuves matérielles manquent, avancer sur modèle hôte/ARM et signaler clairement les limites.

## Critères d'acceptation
- Démonstration traçable de la conservation de l'atomicité et des invariants R0/R1/R2 sous transactionnement incrémental et injections de coupure.
- FIFO/STOP/RTU servis sous charges représentatives avec budgets et marges **mesurés ou bornés**, distinction stricte simulation / cible réelle ; aucune réussite ou temps physique inventé.
- Tests de non-régression V1, métrologie B3 et architecture existante réussis ; aucune capacité successeur annoncée avant intégration complète.
- Rapport explicite des limites, métriques, risques résiduels, choix R5/R6 et preuves reproductibles. Si risque matériel non maîtrisé, ne pas le masquer.

## Publication
Codex développe/committe/push sur main sans force, publie preuves, état/roadmap et DERNIER_RAPPORT.md, puis vérifie lecture distante. ChatGPT contrôlera indépendamment. Pas de demandes de validations répétitives pour actes autorisés ; escalade uniquement structurante, destructive inhabituelle ou physique.
