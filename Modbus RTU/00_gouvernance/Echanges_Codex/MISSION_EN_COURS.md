---
mission_id: TR2-20261013-M015-Q-FIFO-REPEATABILITY-WINDOWS
status: READY
base_ref: main
base_sha: 6ea0468b43aa966a98a4037db5b741a0ec98178f
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M015-Q — Qualifier la répétabilité du FIFO XYZ et augmenter progressivement la fenêtre sur le second banc

## Constat de départ / décision
Le rapport M015-P est publié : sur la seconde NUCLEO, le BIN de contrôle ST strictement identique au M015-O qui ne recevait que 258 timestamps a produit **263 XYZ + 33 timestamps**. Relecture flash et empreinte exactes, zéro erreur de parité/discontinuité/saturation dans cette capture. L'utilisateur a vérifié, débranché et rebranché la nappe entre les essais O/P. **Cause précise non établie** (contact, état initial, alimentation non distingués). La seconde carte et le capteur sont actuellement fonctionnels ; les conserver intacts. Ne pas confondre qualification bornée et garantie de zéro perte ou résolution RCA.

## Préparation impérative
Lire intégralement AGENTS.md, la gouvernance/protocole courant, ETAT_COURANT_TR2.md, DERNIER_RAPPORT M015-P, le comparatif I/J/O/P, les preuves brutes et les sources réellement publiées sur main. Vérifier HEAD réel origin/main, git status, worktrees et stashes avant action. Archiver **sans altération** le contrat M015-P et son rapport conformément au protocole. Ne jamais écraser de travaux locaux ou anciens rapports.

## Objectif et protocole expérimental
1. **Répétabilité sans manipulation physique** : réexécuter la même image de contrôle M015-P et la même configuration, après des resets logiciels contrôlés et documentés, avec plusieurs captures bornées (3 au plus), pour établir si les XYZ reviennent de façon stable. Distinguer reset MCU, reset capteur et coupure d'alimentation : ne pas assimiler un reset GDB à un power cycle mesuré. Aucune intervention manuelle sur nappe/module/alimentation n'est demandée.
2. Avant toute augmentation de fenêtre, établir critères quantifiés : tags bruts/parité, cohérence XYZ distincts, séquence de TAG_CNT, timestamps, registres lus après configuration et cleanup, statut FIFO/overflow, erreurs HAL/SPI, intégrité des buffers, temps et débits réellement mesurés, vérification indépendante de reconstruction des octets. Les métriques ne doivent pas promettre une absence globale de pertes si le protocole ne le démontre pas.
3. **Fenêtres progressives** : si répétabilité confirmée, préparer et qualifier 512 mots XYZ puis, uniquement si tous les garde-fous restent satisfaits, une acquisition bornée de 4096 XYZ sur la seconde NUCLEO. Distinguer mots FIFO totaux et échantillons XYZ, et documenter l'intercalage timestamp. Prévoir lecture segmentée compatible profondeur FIFO et débit SPI/CPU16MHz, contrôle overflow et budget mémoire/temps, sans troncature silencieuse. Respecter les limites réelles du FIFO IIS3DWB et de la capture GDB ; ne pas supposer qu'une fenêtre 4096 tient simultanément dans le FIFO. Privilégier une acquisition continue drainée par morceaux avec traces exploitables.
4. Construire d'abord les tests hôte et ARM ciblés pour chaque changement nécessaire ; garder le binaire de contrôle positif M015-P comme témoin immuable et comparer les empreintes. Ne pas modifier driver ST vendor, firmware TR2 principal M014-H ni interfaces V1 gelées. Réutiliser le code existant, aucun refactoring opportuniste. Éviter les essais combinant plusieurs changements.
5. Si répétabilité échoue ou un garde-fou déclenche, **arrêter immédiatement l'augmentation de fenêtre** ; préserver les captures et décrire exactement l'écart. Ne pas tenter de reproduire un faux contact, de bouger/rebrancher la nappe, d'échanger les capteurs ou de provoquer un état matériel instable. Un éventuel diagnostic d'alimentation/contact nécessitera une mission distincte et des mesures indépendantes, pas une conclusion par association.
6. Le résultat attendu est une **qualification physique bornée reproductible**, pas la résolution définitive de la cause racine O/P. Si 4096 échoue mais 512 est qualifié, conserver explicitement cet acquis et déclarer l'étape 4096 bloquée, sans masquer le résultat positif.

## Autorisations matérielles et garde-fous
`allow_flash: true` et `allow_debug: true` uniquement sur la seconde NUCLEO STLINK SN `0032001F3235510C37333439`, UID `001B00463842501820333157`. **Ne jamais flasher ni debugger la NUCLEO principale** STLINK SN `003500463235510B37333439`, UID `004A003D3842501820333157` dans cette mission. Image/sha/source et identité cible vérifiées avant chaque flash, programmation standard SWD + verify/readback, pas de mass erase, RDP/Option Bytes, restauration/backup 2MiB injustifiés. Les manipulations physiques relèvent de l'utilisateur. Si l'identité cible, la provenance de l'image ou l'état du banc sont incertains, suspendre les opérations physiques. Cleanup sûr et vérifié à chaque essai. Aucun changement matériel implicite.

## Livrables et validation
- Matrice des captures et des resets réellement réalisés, identifiants matériels, binaires/SHAs, traces brutes, analyses indépendantes, mesures de débit, flags FIFO, résultats tests/builds et cleanup.
- Distinguer **VALIDÉ PHYSIQUE BORNÉ**, **NON TESTÉ**, **BLOCKED** et **CAUSE NON ÉTABLIE** ; aucun « zéro perte » sans démonstration de bout en bout.
- Préserver les échecs historiques O et la réussite P sans réinterprétation rétroactive. Actualiser état courant/roadmap et archives. Publier `DERNIER_RAPPORT.md` et preuves versionnées sur main, puis vérifier le HEAD distant et relire les artefacts publiés. Signaler toute erreur de publication.
- Autonomie logicielle/commit/push/flash/debug dans le périmètre sans validations répétées. Ne solliciter l'utilisateur que pour manipulation physique, décision structurante ou risque non maîtrisé.
