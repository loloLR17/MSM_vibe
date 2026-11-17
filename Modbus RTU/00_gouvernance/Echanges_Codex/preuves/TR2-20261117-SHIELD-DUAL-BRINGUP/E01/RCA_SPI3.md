# RCA — mauvaise affectation du MKIGIBV2

## Chronologie démontrable dans Git

Les dates ci-dessous sont celles des commits, pas une heure d’observation matérielle.
Les blobs, empreintes et extraits sont conservés dans [chronologie-RCA.json](chronologie-RCA.json).
Les comptes Git ne permettent pas d’attribuer à eux seuls la rédaction intellectuelle
d’un document ; aucune responsabilité personnelle n’est inférée depuis ce compte.

| Date | Commit / document | Fait établi |
|---|---|---|
|28/09/2026|`74517bef0fae7fcd2420919f8a4a3ff762acf8e5`, `CONCEPTION_FIRMWARE_P12H3G_B_IIS3DWB_SPI3_PINOUT.md`|première occurrence versionnée trouvée : SDO24/SDA23/SCL22/CS21 attribués au schéma officiel ; alimentation1/2 aussi nommée dans l’ordre inverse du dessinST, sans effet de différence de rail puisque toutes deux3V3 |
|12/10/2026|`e222dc8d`, README du banc ST M015-K|les mauvais numéros sont repris dans la préparation du banc autonome |
|12/10/2026|`0eda5384`, rapport M015-L après correction du câblage|WHO7B et polling observés sur le montage à fils ; le rapport ne constitue pas un relevé photographique/ohmique des quatre contacts globaux réellement utilisés |
|02/11/2026|`1d2aa6683e972a3751f69229db4022283f55b3fb`, première CAO shield|matrice et design.json reprennent CS21/SCK22/MOSI23/MISO24 ; schéma, symboles, PCB et CAM sont construits avec cette même erreur |
|05/11/2026|`7557cce1`, V1.9|correction des coordonnées C05 ; les affectations SPI3 historiques sont conservées |
|05/11/2026|V2.3, clôture à`e85c6201`|variante de sérigraphie, électrique préservée explicitement ; mêmes erreurs dans le ZIP candidat |
|17/11/2026|`7c70e7ea4edfe5194198058ab03660073129f285`|dessin ST relu ; mesure001 TP3→SDO=0,3Ω contre≈600kΩ versSCL ; erreur de conception confirmée et alimentation suspendue |

La faute n’a donc pas été créée par le recalage C05 ni par les trois textes ARNACO.
Elle précède le shield et a été **matérialisée dans le cuivre le02/11**. La première
occurrence trouvée n’établit pas la cause de sa rédaction initiale horsGit.

## Mécanisme de propagation établi

Codex a utilisé l’affectation historique comme entrée du schéma au lieu d’en refaire
une référence primaire indépendante. Les vérifications ultérieures comparaient
les artefacts générés à cette même entrée. Les nombres de pads, la parité des nets,
l’ERC/DRC et les attributs CAM pouvaient ainsi tous passer tout en étant faux par
rapport au breakout. Les incertitudes explicitement notées sur l’accès aux documents
ST n’ont pas constitué un arrêt efficace de cette portion électrique.

Le décalage numérique est de+2 sur les quatre signaux JP2 ; les deux contacts NC
23/24 du constructeur ont été utilisés comme données SPI. **Hypothèse**, non cause
initiale démontrée : les deux positions NC au début de la rangée ont été omises lors
de la transcription. Aucun historique de la lecture initiale ne prouve ce mécanisme.
Le fait certain est la réutilisation d’une affectation incorrecte sans son recoupement.

Les succès WHO/XYZ et acquisitions antérieurs ne sont pas annulés. Ils prouvent le
fonctionnement des montages alors testés. Ils ne fixent pas une numérotation globale
du connecteur si le câblage a été effectué par labels de sérigraphie ou autrement :
la manière exacte dont les fils fonctionnels ont été raccordés n’est pas établie
par ces seuls journaux. Il serait faux d’en déduire une conformité de la nouvelle CAO.

## Revues Click et nature des contrôles

- Le contrat shield du02/11 impose explicitement l’examen des pinouts réels, des
  tensions et du Click monté. La matrice produite revendiquait le schéma v102, mais
  portait notamment le libellé incohérent «PWM/DE» tout en routant correctement la
  position3. La citation d’une source n’est pas une preuve de lecture broche à broche.
- La décision précommande du03/11 demande aussi de confirmer les brochages des
  modules, Click inclus. Le contrat V1.9 du05/11 inclut J5/PG4 et modules MKI/Click
  dans les vérifications après recalage. La mission V2.0 demande de contrôler les
  signaux si l’orientation peut remettre en cause le recalageXY.
- Les preuves réellement retrouvées établissent surtout géométrie/mécaniquev101,
  parité interne schéma/PCB/CAM, isolement prévu et réserves matérielles. Elles ne
  contiennent pas une matrice primaire indépendante exhaustive v102 contre tous les
  pads, suivie d’une qualification électrique réelle du Click sur ces shields.
- La revue indépendante V2.3 constate notamment le même blob schéma et autorise
  le ZIP exact pour5prototypes expérimentaux ; c’est une preuve de conservation
  d’une conception, pas une preuve que chaque affectation fabricant était correcte.
- Les demandes orales ou messages non versionnés ne peuvent pas être reconstitués
  exhaustivement depuisGit. Aucune absence de message utilisateur n’est inférée de
  cette lacune. Les exigences versionnées suffisaient déjà à demander le contrôle.

L’audit E-01 actuel distingue le schéma électriquev102 effectivement lu du PcbDoc
et du STEP mécaniquesv101. Il recoupe DE/CS3, /RE/RST2 et les directions réelles de
TXD/RXD du transceiver. Aucun deuxième défaut électrique de pad n’est trouvé surJ9,
mais les straps, contacts, VDDIO2, alimentation réelle et pontJ5 restent non qualifiés.

## Barrières attendues et état final

Le gate E-01 ajouté par ChatGPT exige désormais une référence primaire indépendante,
la comparaison180contacts/17nets, la séparation des preuves géométriques/électriques,
la revue des autres interfaces et une seconde lecture réelle des sources avantGO.
Ces règles n’ont pas été modifiées par Codex. L’audit présent prépare cette lecture,
sans s’attribuer la contre-lecture indépendante ni une libération.

Les notices du dossier V2.3 et de la matrice ont été signalées comme erronées ; les
fichiers historiques sont conservés, aucun CAM définitif corrigé en silence. Reprise
physique, interposeur ou nouvelle fabrication restent une décision après cette revue.
