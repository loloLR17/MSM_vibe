---
mission_id: TR2-20261014-M017-SDMMC-1BIT-REALISTIC-QUALIFICATION
status: READY
base_ref: main
base_sha: 9dd90081ec6e51ec5c6fa84bd5b66f8369717ceb
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M017 — Qualification réelle et reproductible du mode SDMMC2 1 bit pour TR2

## Objectif et décision utilisateur
L'utilisateur demande une **vraie qualification du 1 bit** en conditions représentatives TR2, après réinitialisation de son quota. Exécuter réellement les essais, pas seulement préparer le harness. Ne pas refaire les diagnostics 4 bits ni demander d'oscilloscope (non disponible). Priorité à la **preuve physique** et au diagnostic de l'échec précédent, sans annoncer de succès à l'avance.

## État de référence et échec à expliquer
Lire HEAD main réel, AGENTS.md, GOUVERNANCE_CHATGPT_CODEX.md, dernier rapport M016 FAILED, `experiments/sdmmc_st_reference/LOAD_ONLY.md`, code et preuves `preuves/TR2-20261013-M016-1BIT-LOAD`, rapports C2b/C2c, H3h-A, gel P8-B, état courant. Sur seconde NUCLEO-U575ZI-Q + Adafruit4682 + SanDisk Ultra32Go : M016 1bit/10MHz, producteur 426672 B/s, 64KiB/chunk, ring512KiB, **débordement après 4,164454 s**, 1 310 720 B écrits et relus sans mismatch, débit utile 314740 B/s, latence chunk142–324 ms, HAL errors0. Ancien C2c : 1bit/10MHz, 8x64KiB à54–55ms/chunk, 1 202 495 B/s, zéro mismatch. **La divergence majeure entre les deux campagnes doit être expliquée ou explicitement laissée non résolue.** Ne pas conclure prématurément à un défaut matériel ni à une insuffisance intrinsèque du 1bit.

## Contrat TR2 non négociable
26667 samples/s × 16 octets = **426672 B/s** ; campagnes V1 60 s à 604800 s. Le scénario doit simuler une arrivée cadencée continue, un tampon réellement borné et l'écriture durable avec contrôles d'intégrité. Distinguer bus brut, débit applicatif, blocages du support, buffering et contraintes de RAM du firmware final. Une réussite moyenne seule ne qualifie pas l'absence de perte.

## Périmètre d'exécution, ordre
1. **Reprise sans repartir de zéro** : examiner l'implémentation M016 et les traces, comparer précisément chronométrage, callbacks, DMA linked-list, polling READY, fréquence, taille de chunk, comportement SD, priorité IRQ, charge CPU, producteur et consommateur. Vérifier s'il existe une erreur de méthode, un goulot logiciel ou une différence de mesure C2c/M016. Conserver la provenance ST CubeU5 v1.9.0 et les modifications documentées ; HAL/CMSIS officiels intacts. Ne corriger que des défauts démontrés, avec hypothèse et preuve avant/après. Ne pas contourner la contrainte en gonflant simplement la RAM.
2. **Banc sécurisé** : uniquement seconde Nucleo ST-LINK 0032001F3235510C37333439 / UID 001B0046/38425018/20333157, jamais principal 003500463235510B37333439. Le rapport M016 confirme que firmware SD 1bit sûr a remplacé GPIO et microSD test réinsérée hors tension. Revalider identité, configuration électrique et présence carte, sans demander inutilement de la retirer. MicroSD SanDisk32Go de test sacrifiable, vérifier CID/capacité et zone LBA. Autorisations flash/debug/commit/push permanentes selon AGENTS ; pas de mass erase/Option Bytes/RDP.
3. **Qualification physique progressive et proportionnée** : effectuer un diagnostic ciblé court uniquement si nécessaire pour isoler le goulot, puis un scénario d'arrivée réelle 426672 B/s pendant **au moins 10 minutes**, objectif ~256 MB, en 1bit/10MHz (ou variante strictement justifiée et traçable), avec FIFO bornée dimensionnée selon budget RAM plausible TR2 et latences. Répéter **au moins 3 campagnes complètes identiques** si le premier scénario passe, afin d'évaluer la reproductibilité ; mesurer aussi les échecs, sans retry caché. Les essais peuvent durer plus longtemps si nécessaire et autorisés ; ne pas prétendre avoir validé 7 jours. Prévoir sauvegarde de résultats entre runs et éviter d'écraser des preuves utiles. Ne pas effectuer de benchmark exploratoire sans rapport avec le critère TR2.
4. **Mesures** : timestamps et temps réels, volume généré/accepté/écrit/relus, débit soutenu par run, latences write/READY min/P50/P95/P99/max, distribution et longues pauses, taille backlog max, high-water mark, overflows, pertes, HAL/SDMMC/DMA error et retours, checksum ou comparaison exacte des données sur tout le volume, y compris après réinitialisation contrôlée si faisable. Relecture de la totalité du volume (streaming), pas seulement du préfixe ; preuve de l'absence de perte et d'altération. Garder trace des registres 1bit et horloge effective déduite.
5. **Critères** : pour qualifier le banc en conditions simulées, 3 × 600 s complets à 426672 B/s, zéro perte/overflow, zéro corruption et zéro erreur de transfert, sans backlog croissant non borné, avec latences documentées. Si la première campagne échoue, **arrêter les répétitions**, caractériser précisément la cause et faire au plus un correctif minimal fondé sur preuve suivi d'un nouveau run identique ; si le blocage persiste, publier BLOCKED/FAILED honnête avec preuves et options. Distinguer qualification du démonstrateur de qualification complète TR2 (capteur, codec, checkpoint, power-loss, 7 jours non prouvés).
6. **Autonomie et temps** : pas de nouveau checkpoint final avant d'avoir tenté le RUN ; ne pas interrompre un run pour publier un rapport intermédiaire ; publier le code et le protocole nécessaires puis exécuter la campagne, sauvegarder les preuves et seulement ensuite publier le rapport final. Si session limitée, fournir un runner autonome durable, observable, reprenable et un état précis, sans prétendre qu'une campagne inachevée est qualifiée. Aucun besoin de validation humaine répétitive pour les opérations déjà autorisées. Escalade seulement pour gestes physiques, identité incertaine ou risque matériel réel.

## Livrables et jugement
Rapport `DERNIER_RAPPORT.md` sur main avec statut DONE seulement si critères réellement démontrés, sinon FAILED/BLOCKED/IN_PROGRESS selon faits ; code/commits/hashes, scripts, logs bruts, métriques par campagne, comparaison C2c/M016 et analyse de la cause. ChatGPT contrôlera indépendamment rapport et preuves avant toute décision d'adopter 1 bit pour TR2. Préserver le firmware principal, gels V1, FRAM et supports principaux.

## Permissions
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
