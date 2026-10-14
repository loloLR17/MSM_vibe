---
mission_id: TR2-20261014-P12H3H-SDMMC1-READONLY-FIRSTFAULT
status: IN_PROGRESS
base_ref: main
base_sha: 94fffb0177fbfb6b02731a31a6e2f2ad284bf50b
initial_head: be0864ae11447c544dfb6ee460c8e983aaca0e15
result_sha: 42e8dd236d883366cf37167f9f96293fe95166f4
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# Checkpoint logiciel — probe A préparé, aucun essai physique

**Build A réussi ; zéro flash, reset, accès GDB cible ou commande SD exécuté.** La première erreur n'est pas encore observée. Profil B ni construit ni exécuté. Aucune qualification physique, TR2 ou industrielle annoncée.

## Acquis et preuves

Code isolé dans `experiments/sdmmc1_firstfault/`, source communautaire propre épinglée `1e3c9183bad023ebf7d234134823d9270a0e8b64`. Le générateur produit un overlay HAL/LL et main hors USB/TinyUSB/FatFs, avec [diffs et provenance](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/overlay-provenance.json). Les hooks figent la première erreur en RAM avant effacement des flags, sans instrumentation des boucles FIFO ni modification du recovery original. Les éventuelles transactions internes de HAL_Init après capture restent celles du HAL original : le wrapper arrête la suite au retour, il ne change pas artificiellement cette séquence.

Boot WAIT sans accès SD, trigger RAM unique, UID/CID/capacité/broches/horloges/largeur/SSR gardés. A=1bit/12MHz ; CMD17 512 puis CMD18 8192 au LBA506304 uniquement après phases conformes. Comparaison complète embarquée et décodeur indépendant préparés. Aucun CMD13 ajouté après erreur. Collecte terminale seulement, pas de breakpoint pendant polling. B conditionné par PASS physique complet A.

[Build A attempt2](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/build-A-attempt2.log) : GCC ARM14.2.1, Os/g3, BIN21852octets SHA256 `fb2843742056093610f979268c004bb9c66455cf540069ea5d8f55531a252fb1`, ELF `1d88d7054a82ab75edb3bd7b6f6aa321e4095a3535dbba0107d1cd6cac285b01`. Builddir `/home/lolo/dev/msm/reports/SDMMC1-firstfault-A-build`, preuves locales `/home/lolo/dev/msm/reports/TR2-SDMMC1-FIRSTFAULT-20261014`. Images complètes conservées localement ; sources tiers non modifiées ni réembarquées.

Première tentative compile échouée : hook LL injecté dans des utilitaires DMA sans pointeur SDMMCx ; périmètre du générateur corrigé, [échec conservé](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/build-A-attempt1.log). Deuxième tentative réussie. Contrôles : profil implicite rejeté, py_compile des trois outils, ABI ARM2260/200/84octets vérifiée hors cible, décodeur testé sur frame valide et phase_count invalide. nm ne contient pas les symboles MSC/FatFs/write/erase listés dans artifacts-A.json. Ce contrôle statique ne prouve pas encore une exécution cible.

Validation proportionnée : build ARM du probe réellement concerné, garde de profil et outils hors ligne ; pas de répétition des campagnes ni validations globales de production sur code inchangé. Oracles SHA512 `a9f4d9b4604080bea7999abf2d2874653e71c43a7805bc1735068b1f004f3daf`, SHA8192 `2ac5183f9757f9b08e94e4cf2f4b1ee03a3a66171e8b366989e5b36b93ab563b`, issus du protocole antérieur, non nouvellement relus sur carte.

## Correction de repérage et sécurité

Main concurrent `e02f7c4d969dd94f7761b3c5c8332acbb50af7cb` suspend le matériel sur erreur de repérage alimentation. Manuel ST UM2861 table19 : CN11-16=3V3, CN11-18=5V, CN11-20=GND. Utilisateur corrige « bleu est sur CN11-20 et aubergine sur CN11-16 », puis confirme « ST-LINK déconnecté, bleu sur CN11-20, aubergine sur CN11-16, banc hor tension, SD card non insérée ». [Retours exacts](preuves/TR2-20261014-SDMMC1-FIRSTFAULT/pinout-confirmation.json). Cela étaye le repérage hors tension ; aucune mesure de continuité, de décharge VDD ou inspection visuelle par Codex n'est prétendue. CN15 reste à confirmer explicitement déconnecté. Mission ChatGPT conservée sans modification.

Prochaine étape physique : ST-LINK seul, SD absente, CN15 déconnecté. Vérifier identité seconde cible et image originale avant flash qualifié A, readback et WAIT. Puis insertion de la carte de test hors tension et départ électrique A confirmé utilisateur, mesure VDD0 si possible ou limite documentée. Aucun essai tant que ces conditions ne sont pas réunies. Arrêt définitif du protocole en cas d'échec A ; pas de retry. Si A PASS complet, construire puis exécuter B après coupure utilisateur.

## Préservation et publication

Commit logiciel initial `42e8dd236d883366cf37167f9f96293fe95166f4` préservé. Main a avancé pendant la préparation ; utilisateur autorise explicitement l'intégration isolée. Worktree original de mission intact ; nouveau worktree `MSM_vibe-SDMMC1-firstfault-integrated` basé e02f7c4, réapplication du seul commit logiciel : `8d3db40267d4a1dc9093c9b0e3d9b31819768d69`, arbre source identique, build déjà acquis non répété. `result_sha` désigne le code au build initial ; commit réappliqué identique dans les sept fichiers du probe.

Dépôt principal historique propre `82169af7e76797731358852bec83bfb0ceb54ccf`, stashes `6251b988e8b20f97997a8f98fd26d4d35a45a1b2` et `f3b829e6188f491920edf74a5f9ed0f559d08b87` préservés. ROOTCAUSE-REVIEW archivée byte-identique depuis94fffb0. Journal actualisé : préparation logicielle, correction utilisateur et prochaine étape physique. Principal/FRAM/média512Go/Modbus/protections non accédés. Permissions exercées à ce checkpoint : logiciel et commits ; push normal prévu avec contrôle distant, aucune opération matérielle.

Statut IN_PROGRESS de reprise, pas DONE. Publication et relecture distante de ce checkpoint à effectuer ; SHA publié communiqué après contrôle, sans auto-référence dans ce commit.
