# V1.6 — décision proposée pour l’état visible boîtier fermé

**Priorité : une fenêtre transparente fermée au nord-est du couvercle, en visée oblique devant le bord du shield. Un seul repli : trois LED sur le shield, avec buffers depuis les GPIO existants via Morpho.** Choisir la fenêtre si un contrôle visuel simple confirme la visibilité réelle ; sinon passer au repli, sans développer fibres ou renvois complexes.

**Étude terminée, conclusion conditionnelle ; fabrication non libérée.** Aucun fichier PCB, KiCad, Gerber, firmware ou boîtier de fabrication modifié. [Plan, coupe et schéma de principe — PDF3pages](ETUDE_LED_V1_6.pdf) ; [analyse chiffrée](ANALYSE_DETAILLEE.md), [sources](SOURCES.md), [calculs reproductibles](analyse.py).

| Option | Faisabilité et modifications | Assemblage / coût plausible pour deux prototypes |
|---|---|---|
|**Fenêtre fermée, priorité**|Scénario de visée devant le bord nord, à droite de la nappe, sans découpe PCB. Positions et luminosité C05 inconnues ; vérifier l’observation oblique. Aucun changement électrique/firmware.|Une plaque transparente PC de préférence, cadre vissé, joint, quatre fixations par boîtier. Provision **2–6 EUR/boîtier**, hors impression/port, pas un devis. Pas d’effort appliqué aux LED.|
|Trois guides courts droits|La famille Bivar examinée atteint21,1mm, contre46,6–49,6mm jusqu’au toit dans le scénario actuel. Toit local abaissé et passage PCB à étudier ; couplage non démontré.|Six PLP1-250-F seuls : **4,14USD**, hors porte-guides/adaptations/port/taxes. Alignement et épaisseur du panneau contraints. Non retenu avec cet empilage.|
|**LED déportées, seul repli**|Trois nouvelles prises Morpho, 3LED+3NPN+9R et routage ; pas de changement des clignotements. Position future visible au-dessus du shield.|THT possible, pas de fils volants. Provision **2–5EUR/shield** hors fenêtre/PCB/port ; contacts exacts non sélectionnés. Six NPN seuls affichés **1,02USD**. Plus de soudures et qualification électrique future.|

Ces provisions ne constituent pas un budget livré ; prix/stocks du04/11 dans SOURCES.md. Aucun achat. Le plafond global50EUR reste non démontré.

## Éléments décisifs

**Pas de découpe aveugle.** Un rectangle exploratoire X45..65/Y17..28 touche J8-1/2/23/24 et **+3V3, ACC_MISO, ACC_MOSI**. Ce rectangle n’est pas une position de LED. Le MKI provisoire ne laisse que4,46mm derrière le bord nord ; guides et supports ne sont donc pas une solution automatiquement simple.

**Fenêtre conditionnelle mesurée sur le modèle.** Proposition d’ouverture utile23×10mm, X95..118/Y−16..−6, dans le couvercle z37..40 ; plaque1,5mm et joint, sans indice IP revendiqué. Les1 232rayons calculés évitent les enveloppes disponibles élargies de0,5mm, mais la marge résiduelle au bord du shield n’est que0,246mm. **Le domaine X45..65/Y18..21/Z−12,6..−9,6 est une hypothèse de conception, pas les centres C05.** G11mm est également hypothétique. Visée à48,4–51,7° de la verticale supposée : luminosité, obstacles C05 et tolérances peuvent invalider l’option. USER au sud et RESET protégé sont préservés ; accès USB/SD/RS485 inchangés.

**Repli électrique réellement raccordable, mais pas par Zio.** PC7→CN12-19, PB7→CN11-21, PG2→CN11-42 ; absents de CN7/8/9/10 et du J5 actuel. La population de ces contacts doit être vérifiée ; des mâles supplémentaires pourraient devoir être soudés dans une mission ultérieure. **Ne pas utiliser PA5/D13, déjà FRAM_SCK.** Le schéma C05 et UM2861 confirment HIGH→ON avec drivers NPN et résistances natives. Copier chaque GPIO par Rb4,7k/NPN, Rbe100k et résistance LED1k initiale ; conserver les LED natives. Charge supplémentaire bornée<0,807mA/GPIO et<13,79mA total3V3, hypothèses détaillées dans l’analyse. Vérifier Vf/luminosité, référence NPN et VDDIO2 de PG2 ; GND logique exclusivement, jamais GND2 isolée. Aucun montage parallèle de LED supposé sûr.

## Seule suite nécessaire pour cet arbitrage

Pour la fenêtre : une vue dessus cotée des surfaces émettrices LD1/2/3 relativement à CN8-4, un profil hauteur/obstacles nord avec l’empilage réel, puis lisibilité des trois couleurs/clignotements à travers la plaque depuis une position d’observation utilisable. Ne pas redemander les mesures8,22/2,40 et15,2mm déjà acquises. Aucune manipulation demandée ici.

Si cette vérification échoue : nouvelle mission pour figer les trois contacts et composants, placer/raccorder les LED dans une zone exposée sans déplacer les modules/fixations, vérifier3V3/VDDIO2/isolation, puis ERC/DRC/CAM et revue3D. Les autres réserves V1.5 avant fabrication restent applicables. **Aucune qualification physique ni autorisation de fabriquer ne découle de cette étude.**
