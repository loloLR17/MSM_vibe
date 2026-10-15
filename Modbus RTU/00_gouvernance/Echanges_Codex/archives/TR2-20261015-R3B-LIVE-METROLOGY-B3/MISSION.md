---
mission_id: TR2-20261015-R3B-LIVE-METROLOGY-B3
status: READY
base_ref: main
base_sha: fa2af5606ff7ae939a14a77c8d593da5f38d7e14
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R3-B — Métrologie vibratoire continue, fenêtres et publication B3

## Références et état d'entrée
Lire AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md §10, état/roadmap, contrat R0, LIVE_R0 (PROFIL_LIVE, mapping, tests, décisions), LIVE_R1.md, LIVE_R2.md, LIVE_R3A.md, mission et DERNIER_RAPPORT R3-A sur main. Vérifier HEAD réel, worktrees/stashes et intégrité des gels. Archiver mission et rapport R3-A sans perte. R3-A est reçu comme logiciel seulement : owner FIFO continu et port causal sur SPI simulé ; production reste V1, aucun B3 valide/RTU2 ni mesure physique qualifiée.

## Objectif
Transformer les bursts XYZ contrôlés de R3-A en fenêtres de supervision LIVE et indicateurs B3 corrects, sans microSD, en réutilisant les composants existants. Préparer puis raccorder les projections de registre successeur **seulement si l'ensemble des dépendances réelles est satisfait**. Pas de publication de faux états, de faux échantillons ni de capacité protocole2 anticipée.

## Travaux
1. Relever exactement la norme R0 : fréquences, axes sélectionnés, pleine échelle, unités, RMS/crêtes, seuils warning/alarm, fenêtres, horodatage, validité/âge, compteurs, CRC et registres B3. Dresser les oracles indépendants avant développement ; aucune valeur ou sémantique inventée.
2. Implémenter un accumulateur de fenêtres borné en mémoire et continu, consommant les bursts validés du propriétaire R3-A avec run_id, config_id/génération et axes/FS immuables. Définir frontières exactes, arrondis, saturations, overflow, gaps, timestamp rollover, changement de run, STOP et invalidation. Aucun calcul sur échantillons incomplets ni zéro marqué valide. Préserver l'intégrité XYZ et tester explicitement le masque logique des axes et la saturation d'un axe non sélectionné.
3. Implémenter RMS, pics, seuils et indicateurs B3 avec entiers/arithmétique conformes à R0 ; tests vecteurs numériques indépendants, limites, invariance au découpage des bursts, cadence et fenêtres multiples, longue durée et arrêt/reprise. Éviter allocations ou traitements non bornés en acquisition.
4. Préparer les projections cohérentes B1/B3/B6/B7, B0/B4/B5 successeur et les effets hérités 2..11 selon la norme ; identifier les dépendances et limites. Raccorder au serveur PDU/RTU et à la composition STM32 seulement si le chemin complet est réellement cohérent et sûr ; sinon conserver production V1 et documenter précisément les raccordements restants, sans annoncer protocole2 ou bit LIVE.
5. Examiner **en priorité** la coexistence temporelle du moteur R2 avec le propriétaire FIFO : START/STOP/epoch/APPLY écrivent et relisent de grands volumes FRAM synchrones. Instrumenter les coûts hôte et, si matériel qualifiable, STM32 ; proposer une architecture minimale de budget/admission/ordonnancement qui garantit service FIFO et STOP, sans transformer un budget simulé en preuve physique. Si une modification structurante est indispensable, documenter les options et escalader uniquement cette décision.
6. Appliquer la maîtrise de code R3-A : réutiliser le parseur, transport SPI, stores et oracles ; maintenir inventaire et métriques LOC/ELF, démontrer toute duplication, aucun nettoyage spéculatif ou réécriture globale.

## Tests et preuves
Exécuter les validations complètes hôte et cross-build ARM, tests numériques B3 indépendants, continuité sur >4M XYZ simulés si applicable, fenêtres aux frontières, fautes FIFO/SPI, acquisition/STOP, défaillance FRAM, recovery, non-régression V1 et ASan/UBSan. Fournir mesures mémoire/stack statique et E/S, résultats réellement exécutés et logs versionnés. Distinguer simulation, lien ARM et qualification physique.

## Matériel et autonomie
T1000 seul banc, HP retiré. allow_flash=true et allow_debug=true avec garde-fous AGENTS.md : identification positive carte/UID/image/connexions et sauvegardes avant intervention ; aucune écriture FRAM LIVE [122880,245760) sans preuve de propriété, contenu et sauvegarde vérifiée. Pas de mass erase, Option Bytes/RDP ni changement électrique risqué. L'utilisateur réalise les manipulations physiques. En cas d'incertitude matérielle, continuer hôte/ARM et signaler sans inventer de PASS. Ne pas dépendre de SDMMC.

## Critères d'acceptation et publication
B3/fenêtres conformes R0 avec oracles indépendants et invalidation stricte ; pas de publication LIVE mensongère. Risque FIFO/FRAM explicitement quantifié dans les limites réellement mesurées, avec stratégie R4 concrète. Architecture incrémentale, non-régression V1 et preuves complètes. Mettre à jour état/roadmap, publier DERNIER_RAPPORT.md, commits et preuves sur main par push normal, puis vérifier le contenu distant. ChatGPT fera le contrôle indépendant avant R4/R5. Aucune validation répétitive requise pour actions couvertes par AGENTS.md.
