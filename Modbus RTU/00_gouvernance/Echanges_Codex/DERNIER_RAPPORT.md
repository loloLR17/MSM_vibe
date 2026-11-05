---
mission_id: TR2-20261105-SHIELD-V23-ARNACO-SILKSCREEN
status: DONE
base_ref: main
base_sha: b26c2ac0067112b87015abde9cb196ec8572cf45
initial_head: e5b15d978585c9c83907942528d60fcaaa913265
result_sha: 497a979322018fc56df906ce4866b426b3802e19
created_at_utc: 2026-11-05T18:21:07Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: false
---

# V2.3 — ARNACO © et cinq PCB prototypes

**Personnalisation et dossier candidat publiés et relus depuis la référence distante. DONE signifie mission CAO/exports livrée, pas qualification physique ni autorisation de commande.** GO de préparation CAO/exports pour la revue de cinq prototypes à risque mécanique accepté dans la mission. L'envoi au fabricant, le coût livré, la finition et le paiement restent des décisions humaines. Aucune qualification physique nouvelle et aucun achat.

## Résultat obtenu

Nouveau dossier [V2_3_ARNACO](https://github.com/loloLR17/MSM_vibe/tree/497a979322018fc56df906ce4866b426b3802e19/Modbus%20RTU/04_Architecture/SHIELD_PROTO_V1/V2_3_ARNACO/), dérivé exclusivement du candidat V1.9 C05. Trois textes ajoutés en F.SilkS : **ARNACO**, un **vrai © natif plus petit et surélevé à droite**, puis la devise exacte **« Votre crédulité est notre force »**. Le glyphe et les accents sont visibles dans le rendu du vrai Gerber, pas seulement dans l'éditeur ; aucune substitution `(C)` ni dépendance de police externe.

Emplacement libre entre SD et Click, sous FRAM. Mot-symbole centre(61,8;88), taille2,4/trait0,25 ; © centre(70,5;86,8), taille1,1/trait0,16 ; devise centre(61,8;92), taille1/trait0,15, en mm. Encre ajoutée X50,154..73,351 / Y85,963..92,530. Aucun pad, via, trou, TP, repère fonctionnel ou avertissement recouvert.

Les anciennes annotations V1.9 DRAFT restent en place pour identifier la référence électrique. Aucun changement du schéma, projet, empreintes, cuivre, zones, centres ST, contour, stack, pinout, masques ou contraintes. **Le natif est intégralement identique à V1.9 après retrait des trois nouveaux objets.** Les 505 fichiers antérieurs du shield sont préservés octet pour octet, notamment V1.9/V2.0/D126/V2.2. Les campagnes STM32 sont intactes.

## Contrôles et preuves réellement obtenus

- **KiCad10.0.6 : ERC0, DRC0, non-connectés0, parité0**, `--severity-all`, règles et contrôles ignorés inchangés par rapport à V1.9. Logs/JSON dans `docs/checks/`. Les copies schéma/projet/bibliothèques sont vérifiées identiques.
- **CAM : huit fichiers équivalents à V1.9** — deux cuivres, deux masques, contour, sérigraphie arrière, deux Excellon — après retrait des seules dates de génération et normalisation des fins de ligne. Aucune coordonnée ou instruction de tracé exclue. Les différences brutes initiales de perçage venaient aussi de la ligne `DRILL file ... date`, explicitement prise en compte, pas d'un changement des trous.
- Netlist/X2 et perçages : **59 broches connectées,19 empreintes,191 pads,4 vias**, centres et diamètres contrôlés à la résolution réelle Excellon, contour90×100. Nominal stack vérifié : deux couches, FR4/1,6mm et deux cuivres35µm. Cela ne qualifie pas la fabrication ou la résistance électrique réelle.
- **Sérigraphie réelle : 902 traits anciens conservés,319 ajoutés**, aucun supprimé. Tous les ajouts restent dans les boîtes des trois textes. Distance minimale à l'ouverture de masque la plus proche : **4,2166mm**. La garde de toute la sérigraphie avant est0,1654mm, héritée des anciens textes, à distinguer de la marge du nouveau marquage.
- Enveloppes Fab modules dégagées ; test conservateur d'élargissement FRAM/SD de 2,54 mm en toutes directions : garde marquage/SD≥4,4986mm. Ce n'est ni un emboîtement validé des 1×10, ni une qualification du coffret.
- **Rendus inspectés** : vrai GTO complet et détail ARNACO/©/devise, implantation et cuivre des deux faces (arrière miroir). `preuves/revue-visuelle.json` identifie les PNG lus ; SVG vectoriels et PDF natifs fournis.

Commandes réellement exécutées : personnalisateur natif `tools/customize.py`, exporteur `tools/export_design.py`, readback natif `tools/read_board.py`, `tools/verify_exports.py`, `tools/verify_variant.py` avec bibliothèques Linux déjà présentes, puis `tools/render_review.py`. Les comparateurs et leurs résultats sont dans le dossier. Le lecteur de GTO est volontairement limité aux traits/cercle-flashes rencontrés et échoue sur commande inconnue ; ce n'est pas un validateur Gerber universel.

Aucun test firmware/cross-build/essai physique acquis répété. ERC/DRC ont été exécutés sur **le nouveau candidat** parce que la mission les exige ; les preuves géométriques ST/V2.0/V2.1 inchangées sont réutilisées. Les vérifications de rendu ne sont ni une mesure photométrique ni une preuve d'impression industrielle.

## Dossier prêt pour décision humaine

- Natifs éditables sous `cad/`, avec les bibliothèques locales inchangées et `design.json` d'origine.
- ZIP **`exports/TR2_SHIELD_V23_5PCB_CANDIDAT_NON_LIBERE.zip`**, **neuf fichiers : sept Gerber +deux Excellon**, une seule variante/contour. Manifeste `preuves/CAM-manifeste.json`, SHA-256 **`e91d5ce68ee4339b44ada627fd8fc5dc097b4e9b2a128b15611dd996a6e44d17`** ; archive rouverte et contenus comparés aux exports.
- `FICHE_JLCPCB_5_PROTOTYPES.md` et `configuration-JLCPCB.json` : quantité5, PCB individuels90×100mm, deux couchesFR4/1,6mm, cuivre1oz, masque vert/sérigraphie blanche, **HASL sans plomb à sélectionner**. Prix/délai finaux non actualisés ni acceptés pendant cette mission.
- Point de métadonnées signalé : le `.gbrjob` natif indique Size90,05×100,05 (enveloppe du trait de contour0,05) et FinishNone. Il reste informatif **horsZIP**, sans retouche de la géométrie ; les vrais axes Gerber donnent90×100 et la future commande doit choisir la finition sans plomb. Les deux plans de perçage PDF sont disponibles séparément.

Les [instructions JLCPCB KiCad](https://jlcpcb.com/help/article/how-to-generate-gerber-and-drill-files-in-kicad-6) et les [capacités](https://jlcpcb.com/capabilities/Capabilities) ont été consultées le05/11/2026 pour format/couches/sérigraphie, sans upload. Les nouveaux textes ont une taille nominale≥1mm, trait≥0,15mm ; aucune amélioration de texte ne vaut garantie de qualité de production. Pas de pâte/stencil/assemblageSMT demandé pour ce lot de PCB nus.

## Composants et limites à transmettre

Le contrat rapporte la confirmation utilisateur d'une commande TME **17,60€TTC transport compris**, références/quantités :10×DS1023-1*12S21,10×DS1023-1*10S21,10×DS1023-1*8S21,3×DS1021-2*40SF11,10×MRS16000C1002FCT00,3×SR205C104KAR,3×EEUFR1H100. **Aucune de ces lignes réachetée.** Expédition/réception non vérifiées par Codex ; Samtec exact demandé en échantillons, réception inconnue. Aucun justificatif, numéro de commande, adresse ou donnée personnelle publié.

Les variantes d'assemblage commandées ne sont pas adoptées silencieusement dans les bibliothèques figées. Avant alimentation du premier article : contrôle à froid J5/Zio/G, quatre mâles66/67/68/69 et DE/cinqNC, corps1×10 conservés/patte supplémentaire neutralisée, passifs et enveloppeC1, D126/visserie isolante, modules/étriers, isolementRS485 et accès CN15/ST-LINK/SD/RS485/nappe/USER/LED. Pas de nouvel essai, nouvelle impression ou manipulation demandé maintenant. Risque mécanique prototype accepté distinct de fluage/vibrations/tenue industrielle non qualifiés. L'utilisateur conserve la décision de libération et de dépense.

## Journal et Git

`ETAT_COURANT_TR2.md` actualisé par ajout de l'entrée V2.3 : marquage, preuves/exports, préparation cinq PCB, commande rapportée et réserves physiques. Aucune règle de gouvernance modifiée. Ancien rapport V2.2 conservé à l'identique dans son archive ; mission V2.3 copiée dans son nouveau dossier d'archive, sans modification du contrat actif.

Travail isolé dans `/home/lolo/dev/msm/projects/MSM_vibe-SHIELD-V23-ARNACO`, branche `codex/shield-v23-arnaco`, basé sur le vrai HEAD `e5b15d978585c9c83907942528d60fcaaa913265`. Dépôt principal conservé propre à `82169af7e76797731358852bec83bfb0ceb54ccf`, worktrees et deux stashes historiques préservés. Aucun reset, rebase, merge, force push ou nettoyage destructif.

Permissions exercées : lecture, création du candidat, validations/outillage, documentation, commit et push normal vers `refs/heads/main`. Flash/debug false respectés ; aucun accès STM32, périphérique, microSD, geste physique, achat, panier, paiement ou transfert de Gerber.

Commit de réalisation **`497a979322018fc56df906ce4866b426b3802e19`**, message `feat(shield): add ARNACO superscript copyright and V23 prototype CAM`, poussé normalement depuis `e5b15d97`. Relecture après `git fetch origin main` et contrôle `git ls-remote origin refs/heads/main` : **608 fichiers /55 463 334 octets**, lus intégralement avec `git show origin/main:<chemin>` et comparés aux fichiers locaux/empreintes protégées. Correspondance exacte, dont les 505 fichiers historiques et le ZIP candidat. Rapport provisoire du bon identifiant observé `IN_PROGRESS`. Preuve : `V2_3_ARNACO/preuves/relecture-distante.json`.

La présente clôture et son archive consignent cette relecture. Son SHA est communiqué après le second push et la vérification de la dernière publication ; il ne peut pas figurer dans son propre contenu. Le worktree était propre après le commit de réalisation, puis seules les trois pièces de clôture ont été ajoutées. Les préférences locales KiCad ignorées ne sont pas publiées ; le ZIP candidat, ignoré par la règle générique `*.zip`, a été ajouté explicitement après contrôle de ses neuf contenus et de son empreinte. Le dépôt principal et les deux stashes sont préservés. Aucune libération de fabrication implicite.
