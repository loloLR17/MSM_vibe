---
mission_id: TR2-20261014-P12H3H-SDMMC1-READONLY-FIRSTFAULT
status: BLOCKED
base_ref: main
base_sha: 94fffb0177fbfb6b02731a31a6e2f2ad284bf50b
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# P12/H3h — Isoler la première erreur SDMMC1 par lecture seule hors MSC

## Objectif
Exécuter le protocole borné recommandé par la revue documentaire achevée `TR2-20261014-P12H3H-SDMMC1-ROOTCAUSE-REVIEW`, sans anticiper la cause racine. Distinguer première défaillance SDMMC1, défaillance multibloc et propagation logicielle « No Media ». L'utilisateur valide la poursuite et le lancement de cette mission.

## Référence technique
Appliquer le protocole publié dans `Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261014-SDMMC1-ROOTCAUSE-REVIEW/PROTOCOLE_LECTURE_PROPOSE.md`, après relecture de `AGENTS.md`, `GOUVERNANCE_CHATGPT_CODEX.md`, du rapport de revue et de l'état réel de `main`. Source communautaire épinglée : `VILLO88/NUCLEOU575@1e3c9183bad023ebf7d234134823d9270a0e8b64`.

## Périmètre d'exécution
1. Préparer un probe isolé et instrumenté, compilé et traçable, sans initialiser USB MSC/TinyUSB/FatFs ; conserver séquence HAL SD d'origine, GPIO, alimentation, horloges et fréquence SD 12 MHz. Deux profils comparables : **A=1 bit**, puis **B=4 bits seulement si A est intégralement conforme**. Capturer en RAM la toute première erreur avant effacement des flags, sans perturber le polling ni altérer les chemins de récupération avant observation.
2. Conserver le **câblage Dupont court actuel**, confirmé par l'utilisateur : CN11-16 3V3 aubergine, GND bleu : POSITION NON VALIDÉE (CN11-18 signalé 5V ; CN11-20 signalé GND, à contrôler sur schéma ST et sur la carte réelle), CN11-3 CLK vert/PC12, CN11-4 CMD orange/PD2, CN11-2 D3 rouge/PC11, CN11-1 D2 noir/PC10, CN12-1 D1 marron/PC9, CN12-2 D0 jaune/PC8. Aucun changement CN8, aucune soudure ni changement de strap. ARRÊT DE SÉCURITÉ : aucun flash, aucune mise sous tension ni essai tant que le brochage alimentation CN11-16/CN11-18/CN11-20 n'est pas revérifié sur la documentation constructeur et physiquement confirmé par l'utilisateur. Le précédent tableau de validation ChatGPT est invalide.
3. Banc exclusivement **seconde NUCLEO-U575ZI-Q** : ST-LINK `0032001F3235510C37333439`, UID `001B0046/38425018/20333157`, microSD test SanDisk 32 Go, CID `03534453/44333247/858BF092/3C01927A`, 62333952 secteurs. Exclure strictement Nucleo principale, FRAM, microSD 512 Go, Modbus, Option Bytes, RDP et mass erase. Vérifier cible et empreinte du firmware avant flash, préservation/récupérabilité et procédure ST-LINK qualifiée. Ne pas écrire sur la carte SD.
4. **Actions physiques réservées à l'utilisateur** : éjection sûre du lecteur PC, insertion microSD dans le breakout hors tension, contrôle de décharge VDD breakout/GND proche de 0 V si possible, coupure entre A et B, branchements/débranchements nécessaires. Codex doit préparer tout le logiciel et les preuves possibles avant de demander uniquement ces gestes précis. Ne jamais présumer la carte déjà déplacée ou le banc connecté ; attendre confirmation des gestes nécessaires. Flash et debug logiciel autonomes autorisés, sans validations répétitives.
5. Au plus un départ électrique par profil, CMD17 512 octets puis CMD18 8192 octets sur LBA 506304 si conforme, comparaison octet à octet et SHA-256 aux oracles du protocole. SSR/CMD13 initiaux tracés ; après échec aucune commande de récupération, aucun retry, aucun CMD13 ajouté. Aucun breakpoint/halt pendant polling ; capturer états et données seulement en terminal. Arrêt immédiat sur erreur d'identité, d'init, de lecture, de données ou timeout ; ne pas exécuter B si A échoue.
6. Codex détermine les détails techniques nécessaires à une exécution sûre et reproductible, compile, teste, flash/debug autorisés sur la seule cible qualifiée, conserve les preuves brutes, interprète les limites causales sans transformer un succès borné en qualification industrielle.

## Critères d'acceptation
- Probe A et, si autorisé par succès A, probe B construits avec diff, hashes et logs ; première erreur réellement capturée ou limite documentée.
- Identité carte/cible, paramètres bus, état initial et intégrité des données établis ; respect strict des critères d'arrêt et du nombre maximal d'essais.
- Aucun write SD, aucun MSC Windows, aucun flash d'une autre cible, aucune modification de firmware communautaire amont ni de spécifications gelées.
- Résultat clairement classé PASS borné, FAILED ou BLOCKED si geste physique non réalisé ; ne pas déclarer qualification TR2.
- Rapport `DERNIER_RAPPORT.md`, traces, commits et preuves publiés sur `main` et vérifiés à distance.

## Blocage immédiat du 14/10/2026
L'utilisateur a identifié une erreur critique dans la mission : CN11-18 a été indiqué à tort comme GND, alors qu'il signale CN11-18=5V et CN11-20=GND. Ne pas appliquer l'ancien câblage. La mission reste BLOCKED jusqu'à nouvelle revue indépendante du schéma ST et confirmation physique hors tension de la liaison GND. Les autorisations flash/debug ne constituent pas une levée de ce blocage.

## Arrêt et escalade
Arrêt sur identité douteuse, risque électrique, état matériel inconnu, divergence Git, erreur de lecture, perte de preuve de première erreur, timeout ou nécessité d'une opération destructrice inhabituelle. Préserver les travaux et produire checkpoint `IN_PROGRESS` ou `BLOCKED` si intervention utilisateur nécessaire. Aucun effacement massif, Option Bytes, reset destructif, changement physique autonome, ni test d'endurance. Respecter les autorisations permanentes et garde-fous `AGENTS.md`.

## Livrable
Codex exécute autant que le banc le permet, publie `DERNIER_RAPPORT.md` et preuves vérifiables ; ChatGPT réalise ensuite le contrôle indépendant. Ne pas demander de validations logicielles intermédiaires.
