---
mission_id: TR2-20261012-M015-I-GROUPED-FIFO-SOLUTION
status: READY
base_ref: main
base_sha: c0d4f7b9786921db64f9e031f5458a662ba90332
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-I — Voie FIFO fonctionnelle par vérifications SPI groupées, priorité solution et sauvegarde quota

Respecter AGENTS.md V2-B3, main courant, gels V1, état courant, roadmap et rapport M015-H DONE. Archiver mission/rapport H sans perte. Autorisation permanente commit/push/flash/debug explicite. L'utilisateur ne veut PAS acheter d'oscilloscope, possède Brymen BM869s et un second breakout IIS3DWB, sans intervention physique demandée ici. Priorité absolue : voie d'acquisition exploitable et sûre, non explication exhaustive de la valeur 0x50. Budget restant de session Codex annoncé ~12 % : protéger résultats et continuité avant toute consommation supplémentaire ; si une qualification complète paraît longue, s'arrêter après préparation et publication d'un état reprenable plutôt que risquer la perte des acquis. Ne pas prétendre pouvoir anticiper exactement la coupure du quota.

## Objectif
Adapter au minimum la vérification de configuration du harness FIFO F/G pour utiliser des lectures groupées avec IF_INC et contrôle de cohérence des registres critiques (CTRL1_XL=0xA8 et contrôles FIFO), sans lectures simples problématiques sur le chemin d'admission. Une lecture groupée n'est PAS réputée fiable par principe : exiger des valeurs cohérentes, des gardes de transfert, et fail-closed au premier écart. Si configuration vérifiée, effectuer au maximum une acquisition FIFO RAM courte et bornée (p. ex. bursts128 déjà préparés) et rapporter vrais mots/tags/XYZ/erreurs/timing ; ne pas assimiler cet essai à la continuité V1. Si une lecture groupée échoue, restaurer et clôturer BLOCKED avec preuve ; ne pas repartir en campagne d'analyse générale ni masquer une erreur.

## Plan à budget maîtrisé
1. PRÉSERVATION IMMÉDIATE : vérifier branche/HEAD, statut worktrees/stashes et rapport H ; sauvegarder et archiver les preuves H, identifier les fichiers critiques et l'image permanente M014-H. Avant tout flash ou long travail, créer un checkpoint Git poussé sur main contenant plan court, modifications déjà valides et tests disponibles, sans publier un rapport de succès prématuré. Consigner SHA et éléments restant à faire. Pas de fichiers temporaires secrets/énormes en Git ; archives brutes locales sous /home/lolo/dev/msm/reports avec manifeste/hash.
2. Réutiliser le harness FIFO et tests existants, modifier uniquement la stratégie de lecture de contrôle vers un ou plusieurs bursts groupés permettant de vérifier les adresses/valeurs attendues et registres voisins. Documenter l'invariant et les limites : IF_INC, CTRL1=A8, FIFO controls, HAL_OK non suffisant, pas de vote/majorité/fallback sur erreurs, contrôle des tags et perte. Tests hôte ciblés + cross-build ARM défaut/profil ; aucun refactoring ou nouvelle campagne d'analyse SSSS/GS. Checkpoint push après validations, avec hashes, même si la phase physique est reportée.
3. Si le temps/quota disponible permet raisonnablement la phase matérielle ET les préflights sont sûrs : une seule campagne physique courte, flash standard NORMAL après UID/image et backup Flash2Mio/RAM utile ; jamais HOTPLUG, mass erase, OptionBytes/RDP. Dernier CLI puis reset GDB qualifié, debugger détaché pendant l'essai. Traces SPI/config/FIFO/XYZ horodatées, données réelles et limites ; arrêter immédiatement en cas d'anomalie ou d'incohérence. Aucun changement matériel, FRAM ou microSD. Restaurer impérativement l'image permanente M014-H en finally, Flash2Mio byte-identique, vérifier reprise HP B0/B1, FRAM gen12/SD OFF et MCU running. En cas d'impossibilité de garantir restauration complète avant coupure quota, NE PAS commencer le flash : reporter la campagne physique à la prochaine session.
4. PUBLICATION PRIORITAIRE AVANT ÉPUISEMENT : pousser sur main les changements validés, preuves et manifeste de hashes, rapport DERNIER_RAPPORT.md avec statut DONE/BLOCKED/IN_PROGRESS véridique, HEAD/SHA, résultats réellement obtenus, commandes, sauvegardes, état du STM32, risques, prochaine commande/étape pour reprise. Actualiser ETAT_COURANT et roadmap proportionnellement. Vérifier distant GitHub par lecture et consigner preuve de readback. Réserver du budget pour ce push/readback et la restauration physique ; arrêter les analyses secondaires dès que nécessaire. Si quota imminent, privilégier sauvegarde locale des traces et push d'un rapport de reprise minimal exact avant toute expérimentation supplémentaire. Une fin de quota n'est pas un succès technique.
5. Si travail long : clôturer cette session proprement après préparation logicielle et publication de l'état reprenable ; attendre renouvellement quota pour la campagne physique, sans demander validation utilisateur supplémentaire pour les opérations déjà autorisées.

## Invariants
ODR 26667Hz, fenêtres 4096..32768, continuité/store-and-forward et mapping V1 gelés. Aucune réduction implicite des critères, aucune admission sur registre incohérent. Aucun achat d'instrument, remplacement du breakout, changement de câblage, programmation persistante FRAM/SD, campagne FIFO répétée, DMA/FFT/Web hors scope. Escalade seulement pour geste physique indispensable, risque matériel incontrôlé ou décision structurante. Publier preuve positive OU blocage précis, pas de supposition.
