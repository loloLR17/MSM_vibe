# Revue indépendante ChatGPT — Shield V2.4 SPI3 correctif, lot CAM prototype

Date : 2026-11-18 ; branche `main`. Mission `TR2-20261118-SHIELD-V24-SPI3-GERBER-CORRECTIF`, rapport Codex `DONE`. Commits techniques `dc4ac0414db6795796a1649a9433d4f543d8a310`, publication ZIP `2ad3852b4e87af7b24f98d871f59efcd69aa0969`, clôture `f1b168aa4f8f09326c0245ef4a7e919b739f1411`.

## Décision

**GO DE REVUE TECHNIQUE BORNÉ POUR LE CANDIDAT PCB NU V2.4, EN VUE D'UNE FABRICATION DE PROTOTYPES UNIQUEMENT**, après décision utilisateur sur la commande. Le dossier numérique V2.4 peut être identifié comme candidat corrigé à transmettre au fabricant **uniquement avec son ZIP précis vérifié et les options de fabrication contrôlées**. Aucun paiement, transfert fournisseur, achat ni nouvelle fabrication exécuté par ChatGPT. Cette décision ne qualifie pas un shield assemblé, le capteur réparé, la tenue industrielle, ni les modules réels. Les V2.3 existantes restent défectueuses sur J8.

**Gates numériques E-01/M-01 : acceptés dans le périmètre des sources gelées et contrôles ci-dessous.** Les réserves de fabrication et premier article restent obligatoires. Pas de tag de qualification physique.

## Vérifications indépendantes effectives

1. Lecture distante de `AGENTS.md`, gouvernance E-01/M-01, contrat, rapport `DONE`, sources V2.3/V2.4, contrôles, inventaires CAM, matrices et documents V2.7. Le rapport ne vaut pas à lui seul un GO.
2. **Inspection visuelle directe du schéma officiel ST** `https://www.st.com/resource/en/schematic_pack/steval-mki208v1k_schematic.pdf` (figure 2) : JP2 = 19 CS, 20 SCL, 21 SDA, 22 SDO, 23/24 non connectés. Les assignations V2.4 correspondent ; la précédente erreur +2 est corrigée.
3. Lecture directe des deux fichiers natifs KiCad `V2_3_ARNACO/cad/TR2_Shield_Proto_V1.kicad_pcb` et `V2_4_SPI3_CORRECTIF/cad/TR2_Shield_Proto_V1.kicad_pcb` : **180 contacts numérotés comparés** pour leurs numéros, positions locales et nets : **174 inchangés, exactement six réaffectations J8-19..24**, coordonnées des six pads identiques. J8: 19 ACC_CS,20 ACC_SCK,21 ACC_MOSI,22 ACC_MISO,23/24 NC. J6, J7, J9 sans changement. **19 empreintes** : centres, faces et rotations extraits indépendamment du PCB et comparés V2.3/V2.4, concordants.
4. Relecture et analyse du CSV E-01 complet publié : **180 lignes, 180 PASS documentaires** ; six lignes J8 conformes sur colonnes fonction constructeur, schéma, PCB et les deux cuivres CAM. Les 17 nets publiés comprennent le circuit SPI3 et préservent RS485 Click/LPUART/DE, FRAM et SD. Cela n'est pas une mesure électrique sur une carte V2.4.
5. Relecture directe des résultats `M01-native.json`, `M01-STEP-V27.json`, huit tests négatifs M-01 rejetés. Le rapport Codex établit 191 géométries de pads/trous, 4 vias et interfaces STEP V2.7 gelées conservées, avec aucun autre net/cuivre modifié que SPI3. Le contrôle direct de ChatGPT porte sur les positions/affectations extraites et non sur la reproduction indépendante intégrale de CadQuery/STEP.
6. **Lecture du ZIP binaire GitHub lui-même**, décodage base64 indépendant, lecture du central directory : taille **136716 octets**, **9 membres exactement** (F/B Cu, F/B mask, F/B sérigraphie, Edge Cuts, PTH et NPTH), mêmes noms que le manifeste ; calcul SHA256 direct indépendant **`d2a199a1bf3b9d9533bfe788b144510bb8cfd518dc248f05df2c2a644854e7ec`**, concordant au rapport. Les CRC/décompressions de tous les membres et leur comparaison octet à octet aux CAM séparés ont été exécutées et rapportées par Codex, mais non répétées intégralement par ChatGPT.

## Réserves avant et après la commande

- Les 180 PASS sont **des conclusions numériques** : aucune V2.4 n'existe physiquement ; pas de qualification des soudures, des rails, de l'alimentation en charge, des contacts ou des vibrations. Le circuit Click physique a été démontré sur **l'ancien TR2-001 V2.3**, dont les nets ont été préservés, et non sur une V2.4.
- **M-01 : aucune modification mécanique permise**. Le gel V2.7 (pièces PLA déjà imprimées) demeure ; montage à froid requis au premier article. Réserve J5 : corps STEP simplifié 7,68 contre 8,13 mm Samtec, pas de collision numérique avec le corps constructeur, ajustage/contact réel à vérifier. Les 1×10 remplaçant les 1×9 ont un dixième contact à neutraliser lors du futur montage ; compatibilité physique/queues à examiner à froid.
- La documentation native conserve volontairement le texte de sérigraphie `V1.9 DRAFT` et `ARNACO` pour garantir M-01 : **l'identité du fichier à fabriquer est celle du chemin V2.4 et du SHA ZIP ci-dessus, pas du texte historiquement imprimé sur la carte**. Ne jamais transmettre un ZIP V2.3 ou un lot mélangé.
- Vérifier lors du devis fabricant dimensions 90×100 mm (axes réels), 2 couches FR4 1,6 mm, cuivre 1 oz, finition HASL sans plomb, contour, perçages et aperçu avant paiement. Le `.gbrjob` indicatif 90,05×100,05 et `Finish:None` ne définissent pas ces options.
- Le lot de composants doit être commandé séparément et rapidement en distinguant stock et délais réellement garantis. Les livraisons avant samedi 21/11/2026 ne sont pas prouvées. Ne pas présumer que les connecteurs déjà soudés peuvent être récupérés.
- Après fabrication : contrôles hors tension et montage PLA réels, continuités SPI3 broche par broche, absence de courts-circuits et de déformation, gate rails, puis essais WHO7B/FIFO et intégration progressive. Aucun test logiciel sur une V2.4 fictive.

## Autorisation pratique

**Candidat identifié pour une décision de commande utilisateur :**
`Modbus RTU/04_Architecture/SHIELD_PROTO_V1/V2_4_SPI3_CORRECTIF/exports/TR2_SHIELD_V24_SPI3_CANDIDAT_NON_LIBERE.zip`.
L'appellation `NON_LIBERE` reste conservée dans l'artefact historique de mission ; la présente revue autorise **la proposition à la commande de PCB prototypes**, sans achat automatique ni présomption d'acceptation par le fabricant. La mise en service industrielle et la qualification système restent NO-GO.
