---
mission_id: TR2-20261015-R3A-ACQUISITION-LIVE-ARCHITECTURE
status: READY
base_ref: main
base_sha: 3f3ca1c38742c277de349b789e3e82fd52a1c791
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-A — Acquisition continue LIVE et maîtrise de l'architecture firmware

## Contexte
R0 norme et R1 configuration/FRAM sont livrés ; R2 a livré moteur B5 epoch/START/STOP testé hôte mais non lié à la composition STM32 de production. Production reste protocole 1. La croissance R1+R2 atteint +2727 lignes nettes **incluant tests et documentation**, sans preuve d'un code mort massif. Risque à maîtriser : maintenir deux chaînes parallèles ou du code non intégré. Le présent lot R3-A prépare l'acquisition continue et exige une revue architecturale **intégrée au travail**, pas une refonte administrative.

## Références et méthode
Avant toute action : consulter AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md (notamment §10 : banc T1000, HP retiré), état courant, roadmap, contrat R0, LIVE_R0 (profil, mapping, scénarios), LIVE_R1.md, LIVE_R2.md, mission et rapport R2 ; vérifier HEAD distant et état Git. Archiver la mission et le rapport R2 sans perte, préserver les gels V1/V1.1, worktrees, stashes, preuves et branche main. Appliquer A-06 : réutilisation maximale, changements incrémentaux, pas de réécriture globale.

## Objectif technique R3-A
Concevoir et implémenter, sur les composants STM32/IIS3DWB/FIFO existants, le propriétaire d'acquisition **continue sans microSD** avec START/STOP causal, association run_id/configuration/génération et observation effective de l'état. Il doit pouvoir satisfaire l'interface LiveAcquisitionPort de R2, sans fabriquer une preuve de fonctionnement ni prétendre que le profil 2 est exposé par Modbus tant que la composition complète ne l'est pas. Préparer R3-B : fenêtres, RMS/crêtes/seuils, B3 et raccordement RTU2 cohérent.

## Exigence de maîtrise du code (obligatoire, proportionnée)
1. Cartographier les chemins existants : acquisition V1 et FIFO qualifiée bornée, composition STM32, B5 historique, stores, R1/R2, Modbus/PC ; classer chaque module comme lié à l'ELF de production, utilisé seulement par tests, préparé pour intégration, historique requis, candidat à retrait. Prouver les classifications par CMake/linker/références, sans qualifier « mort » un module seulement non lié aujourd'hui.
2. Produire un tableau mesuré de lignes physiques production C/C++ (hors générés/vendor), tests, documentation pour baseline avant R1, HEAD R2 et résultat R3-A, avec méthode reproductible et distinction ajouts Git/LOC nettes ; taille FLASH/RAM et dépendances si mesurables. Identifier duplications réelles (moteurs B5, médias FRAM, source vibration, chemins FIFO) et les choix de mutualisation.
3. Réutiliser prioritairement les pilotes, ordonnancement, gestion d'erreur et types déjà qualifiés ; supprimer ou simplifier seulement les duplications démontrées, après tests. Ne pas effacer le code historique nécessaire à V1 ou les preuves. Pas de refonte générale, pas de réduction artificielle de LOC ni de nouveaux frameworks.
4. Livrer dans le rapport une matrice « conservé / mutualisé / remplacé / différé » et une liste limitée de dettes prouvées, classées selon risque réel.

## Travaux fonctionnels
- Examiner les contraintes de la capture FIFO existante et du bus SPI, les fenêtres R0 et le contrat causal R2. Créer le plus petit service continu réutilisant les chemins HAL existants, avec un propriétaire unique, état réel, run_id et configuration figés par exécution ; absence SD/DataStore/campaign_id dans le LIVE.
- START ne réussit que si l'acquisition est réellement armée et observable, STOP doit être sûr, idempotent et observable même après défaut FRAM ; reboot sans auto-START. Gérer les transitions, débordements FIFO, gaps, erreurs capteur et refus sans émettre de faux résultats valides.
- Prévoir les interfaces de publication des échantillons/fenêtres pour R3-B sans fabriquer RMS/B3. Le raccordement au moteur R2 est autorisé si les preuves et la composition le permettent ; sinon exposer explicitement les limites et laisser les capacités non annoncées.
- Tester hôte avec ports simulés, transitions/échecs/coupures, non-régression V1 et cross-build ARM. Lorsque réalisable, tests ciblés STM32 T1000 avec preuves distinctes ; aucun PASS physique présumé.

## Sécurité matérielle et autonomie
allow_flash=true et allow_debug=true selon AGENTS.md ; Codex peut flasher/debugger sur T1000 après identification positive de la cible, vérification des connexions/alimentation et sauvegardes appropriées. Ne pas écrire la partition FRAM LIVE [122880,245760) avant preuve de propriété et contenu, et sauvegarde vérifiée ; ne pas modifier Option Bytes/RDP, effacer des zones inconnues ou réécrire l'historique. Le HP est hors banc. Les interventions physiques manuelles restent à l'utilisateur. Si matériel absent ou non sûr, poursuivre la validation hôte/ARM et signaler la limite, sans bloquer le travail logiciel.

## Critères d'acceptation
- Cartographie/mesures LOC/ELF et analyse de duplication traçables, proportionnées et reproductibles ; aucun code déclaré mort sans preuve.
- Acquisition continue et START/STOP causal développés dans le périmètre démontrable ; transitions et refus prouvés, pas de campagne ni dépendance SD en LIVE.
- Tests pertinents et non-régression V1 réussis, preuves versionnées et empreintes ; distinction explicite logiciel vs STM32 physique.
- Aucun profil protocole 2 ou B3 valide annoncé prématurément ; limites R3-B/R4/R6 documentées et roadmap actualisée.

## Publication et clôture
Codex analyse, développe, teste, commit/push normalement sur main, publie DERNIER_RAPPORT.md avec commits, preuves, métriques, limites et recommandation R3-B, puis vérifie la publication distante. ChatGPT effectuera un contrôle indépendant. Ne pas demander de validations intermédiaires pour les actions couvertes par AGENTS.md ; escalader seulement décisions structurantes, opérations destructives inhabituelles ou risques matériels non maîtrisés.
