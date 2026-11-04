# V1.8 — décision unique avant commande des cinq PCB

**Verdict courant : NO-GO pour l’envoi des Gerber, en attente du contrôle physique regroupé C05/empilage.** L’audit logiciel est terminé ; aucun défaut démontré ne justifie une modification du PCB. L’utilisateur a accepté le contrôle unique puis l’a reporté à **demain** (retour du04/11) ; **aucun résultat physique n’est acquis**. Le prix TTC rendu complet et la date du lot ne sont pas vérifiés : ne pas transformer le tarif d’appel en devis.

Référence examinée : `6d6417a86d7bdb2a725bd5557bbd4dcac413b7f8`, candidat électrique **V1.4**, projets [KiCad](../cad/TR2_Shield_Proto_V1.kicad_pro), [schéma](../cad/TR2_Shield_Proto_V1.kicad_sch), [PCB](../cad/TR2_Shield_Proto_V1.kicad_pcb). Aucune reconception ; V1.5/V1.6/V1.7 et campagnes conservées. **Pas de READY_TO_ORDER ni de commande par Codex.**

## Matrice de libération

| Interface | Décision et preuve actuelle | Seul complément nécessaire |
|---|---|---|
|Schéma/connectivité, règles et routage|**GO documentaire/électrique** : ERC0, DRC0, non-connectés0, parité0,19empreintes/191pads/59connexions/4vias ; comparaison netlist/PCB/CAM indépendante réussie.|Qualification du prototype assemblé ultérieure, pas de nouvelle campagne STM32 avant PCB.|
|J5 logique/format|**GO logique** : femelle Samtec2×3 dessous, local5=CN12-69/PG4→DE, cinq autresNC indépendants ; reçoit les mâles en quinconce66/67/68/69 sans modifier Nucleo.|Pas de substitution au Samtec choisi ni inversion d’une vue miroir.|
|C05, Zio/Morpho, G et fixations|**NO-GO physique regroupé**, alignement des deux cartes et tolérances non certifiés. ÀG11hypothétique : J5 insertion5,73 mais jeu entre plastiques seulement0,09 ; entretoiseØ4 ne garde que0,04 nominal prèsZio.|Gabarits100% sur les deux C05 ; G naturel et incertitude seulement si témoins exacts disponibles, dégagement corps/queues/vis sans force. Les mentions C05 des deux cartes ne prouvent pas leur ajustement.|
|MKI, Click, supports et isolation|**GO des interfaces documentaires**, mais inclus dans le **même contrôle physique** avantPCB : entraxeMKI15,2 acquis, format retenu15,24 ; contours réels/révisionClick et zones d’appui à confronter. AncrageClick94,5/112,5 conservé, GND2 absent du shield.|Comparer les volumes et ancrages au gabarit, sans démontage des montages validés ni pont mécanique conducteur vers le côté isolé.|
|LED latérales côté ST-LINK|**Pas de conflit PCB incontournable démontré**, mais visibilité conjointe LD1/2/3 dans l’empilage encore dans le **même contrôle physique**. Les rayons V1.7 restent des scénarios, pas trois LED localisées.|Une même zone de vision des trois voyants utiles, port ST-LINK et MKI/nappe en place ou gabarit réaliste au bon G. Si compatible, la forme du vitrage sera une réserve **de coffret seulement**.|
|USER/RESET|**GO de principe sous réserve de coffret** : priorité à un **accès direct protégé à B1** par petite trappe côté CN15, sans pièce pressant au repos. Aucun mécanisme V1.7 imposé. Le couloir réel reste à confirmer dans le **même contrôle**.|Conserver capuchon, hauteur10,2/clic≈0,4 acquis ; RESET hors accès. Si une pièce à collerette devient utile, un seul poussoir, garde et butée structurelle simples à vérifier, pas de ressort réglé présumé sûr.|
|USB ST-LINK/CN15, extractionSD, RS485, nappe|**GO sous réserves de coffret uniquement dans l’enveloppe nominale** ; accès par parois/trappes ajustables. Aucun reroutage/contour PCB requis par les preuves actuelles.|Le contrôle regroupé doit exclure une interférence réelle avec shield/supports avant de reporter les ajustements au PETG. État de la SD non changé pendant l’étude.|
|Fabrication/CAM|**GO des exports candidats contrôlés**, pas de libération physique :90×100,2couches,1,6mm,35µm visés, vias0,3/0,6 ; trous PTH/NPTH concordants, écartsPTH≥1,2mm.|Avis physique ci-dessus puis revue indépendante ChatGPT et décision humaine avant transfert au fabricant.|
|BOM/approvisionnement|**PRÉPARÉ, coût rendu/date NON VÉRIFIÉS**. [BOM complète](BOM_2_PROTOTYPES.csv), [stocks/sources/taxes](SOURCES_ET_COUTS.md), [calculs](budget-TTC.json).|Devis fournisseur sans accepter de commande, prixFrance/port/taxes/canalDDP et longueurs de fixations après G ; aucun coût inconnu mis à zéro.|

## Ce qui doit être réglé avant PCB, ce qui peut attendre

**Un seul retour physique regroupé** suffit : [PDF3pages avec deux gabarits1:1](CONTROLE_UNIQUE_C05.pdf), [texte compact](PROTOCOLE_PHYSIQUE_UNIQUE.txt). Il regroupe les alignements et G des deuxC05, l’appui modules/fixations, les trois voyants et le couloirUSER, ainsi que les accès. Aucune nouvelle mesure8,22/2,40/15,2 ni10,2/0,4 demandée. Sans connecteurs témoins déjà disponibles, noterGnonmesurable ; le papier ne peut pas certifier l’insertion et il ne faut ni forcer ni acheter pour fabriquer artificiellement une preuve.

Une fois la compatibilité au shield démontrée, **vitrage latéral PC, dimensions exactes de la trappe USER, panneaux/serre-câbles et découpes PETG** peuvent être ajustés ultérieurement. Ne pas remettre la force d’un mécanisme complexe comme gate de commande si l’accès direct protégé suffit. Aucun centreB1 fictif adopté. Le guide illustratif V1.7 reste archivé, pas sélectionné.

Un conflit réel de centres/engagement/appui métallique ou une obstruction nécessitant une découpe du shield empêcherait la libération jusqu’à correction minimale et nouveau contrôle ; **aucun de ces défauts n’est actuellement démontré**, et aucun PCB n’est modifié dans cette revue.

## Fabrication et préparation de commande

Jeu candidat [ZIP NON LIBÉRÉ](TR2_SHIELD_V14_NON_LIBERE_V18.zip), [manifeste SHA256](CAM-manifeste.json), [répertoireCAM](CAM_CANDIDAT_NON_LIBERE/). ZIP :10fichiers (7couches,2ExcellonPTH/NPTH,job), réouvert et chaque fichier comparé. Les nouveaux exports sont réconciliés avec la netlist et le PCB, pas des fichiers simplement renommés depuis une vieille archive. Les anciens CAM restent intacts.

[Configuration5PCB](CONFIGURATION_5PCB.json) : cinq cartes simplesFR42couches,90×100,1,6mm,cuivre1oz,masquevert/deuxsérigraphiesblanches,HASLsansplomb proposé, aucun assemblage/stencil/panneau/impédance contrôlée. Les capacitésJLC consultées correspondent nominalement ; estimation publique France obtenue :5,30USD de fabrication, port7,33/16,11/23,44USD selon10–12/6–9/2–4jours ouvrables annoncés, production2jours. Ni DFM sur Gerber ni facture TTC finale obtenus. Voir les sources et la preuve du calculateur ; finir le devis après les gates, sans commander.

**Option d’approvisionnement privilégiée** : TME+DigiKey, troisJ5 dont un secours, cinqfemelles9 dont une réserve, passifs avec marge raisonnable et MOQ des Connfly. Variantes Harwin9/PanasonicFR détaillées et vérifiées nominalement, **non adoptées silencieusement** ; BOM active et CAO conservées. Composants+ports connus convertis≈**53,04..58,04EUR TTC indicatifs**, **plus PCB rendu et mécanique/coffret**. Scénario troisdistributeursmoinscher en articles mais transport/importTayda inconnus : pas retenu comme économie démontrée. Dates de réception complètes et totalTTC restent `null`, pas annoncés comme acquis.

## Portée des contrôles et avertissement de revue

[Synthèse](preuves/audit-synthese.json) et preuves KiCad/CAM versionnées. Les DRC/ERC utilisent toutes les sévérités **des contrôles activés**, sans modifier le projet. Leurs listes de contrôles ignorés par défaut sont conservées : filtres, simulation, certaines conventions, dont centrage des vias. Tous les19courtyards sont présents dans un contrôle complémentaire.

**Diagnostic complémentaire corrigé, preuves conservées** : une première sonde trop stricte **échoue** sur le sommet intermédiaireACC_SCK(49,9;37,5), à0,1mm du via(49,8;37,5). La lecture de la route et du PCB montre ensuite le segmentB.Cu de0,1mm allant exactement au centre du via. L’hypothèse «tout sommet interne au disque du via doit être son centre» était excessive ; ce n’était pas une entrée de via décentrée. Le [contrôle approprié](preuves/verification-entree-via.json) vérifie pour **chacun des quatre vias une terminaison centrée de même net sur chacune des deux couches** :PASS. La sonde initiale/logs d’échec restent conservés, sans prétendre qu’elle a passé ; **aucun PCB, réglage DRC ou résultat attendu n’a été modifié pour la faire réussir**. Le motif deNO-GO reste la preuve physique manquante, pas cette fausse alerte expliquée.

La décision de commande appartient à l’utilisateur après revue ChatGPT. Cette matrice est un audit précommande et une préparation d’approvisionnement, **pas une qualification physique ni un achat**.
