#include "geneticalgorithm.h"

template<class T>
GeneticAlgorithm<T>::GeneticAlgorithm(uint gsize,uint psize,Farbe ** picture,uint picx,uint picy)
                :m_populationsize(psize), m_genomesize(gsize), picsizex(picx), picsizey(picy)
{
    pic_compare = picture;
    render_pic = new Farbe*[picx];
    for(uint i = 0;i<picx;i++)
        render_pic[i]  = new Farbe[picy];

    best_pic = new Farbe*[picx];
    for(uint i = 0;i<picx;i++)
        best_pic[i]  = new Farbe[picy];

    m_individuals = new individual[psize+3];
    for(uint i = 0;i<psize+3;i++)
    {
        m_individuals[i].gene = new T[gsize];
    }
    srand(time(NULL));
    blackwhite = false;
    startsize = 15;
    fitn = new uint64_t[m_populationsize];
    varbuffer_changed = false;
}

template<class T>
void GeneticAlgorithm<T>::init_variables(bool bw,int size,int spawn,uint tnum,uint fitmode,uint msize,uint mrate,uint xtype)
{
    fitnessmode = fitmode;
    threadnum = tnum;
    blackwhite = bw;
    startsize = size;
    mutsize = msize;
    mutrate = mrate;
    xovertype = xtype;
    spawnmode = spawn;

    for(uint i = 0;i<m_populationsize+3;i++)
    {
        random_assign(i,spawn);
    }
}

template<class T>
void GeneticAlgorithm<T>::reset_progres()
{
    for(uint i = 0;i<m_populationsize+3;i++)
    {
        random_assign(i,spawnmode);
    }
}

template<class T>
void GeneticAlgorithm<T>::set_extra_variables(uint tnum,uint fitmode,uint msize,uint mcount,uint xtype)
{
    varbuffer_action.lock();
    varbuffer[0] = tnum;
    varbuffer[1] = fitmode;
    varbuffer[2] = msize;
    varbuffer[3] = mcount;
    varbuffer[4] = xtype;
    varbuffer_changed = true;
    varbuffer_action.unlock();
}

template<class T>
GeneticAlgorithm<T>::~GeneticAlgorithm()
{
    for(uint i = 0;i<m_populationsize+3;i++)
    {
        delete [] m_individuals[i].gene;
    }
    delete [] m_individuals;

    for(uint i = 0;i<picsizex;i++)
        delete [] render_pic[i];
    delete [] render_pic;

    for(uint i = 0;i<picsizex;i++)
        delete [] best_pic[i];
    delete [] best_pic;
    delete [] fitn;
}

template<class T>
void GeneticAlgorithm<T>::bestpic_to_act()
{
    bestpic_action.lock();
    for(uint x = 0;x<picsizex;x++)
        for(uint y = 0;y<picsizey;y++)
            best_pic[x][y] = render_pic[x][y];
    bestpic_action.unlock();
}

template<class T>
void GeneticAlgorithm<T>::mutate_all()
{
    for(uint i = 0;i<m_populationsize;i++)
    {
        if(rand()%5 < 4)
        {
            uint8_t v = rand()%9;
            for(uint b = 0;b<v;b++)
                mutate_single(i,rand()%m_genomesize);
        }
    }
}

template<class T>
void GeneticAlgorithm<T>::change_position(uint individual,uint p1,uint p2)
{
    T zwischen = m_individuals[individual].gene[p1];
    m_individuals[individual].gene[p1] = m_individuals[individual].gene[p2];
    m_individuals[individual].gene[p1] = zwischen;
}

template<class T>
void GeneticAlgorithm<T>::mutate_single(uint individual,uint gene)
{
    cout <<"Dieser Datentyp wird nicht unterstützt!"<<endl;
}

template<>
void GeneticAlgorithm<TriangleData>::mutate_single(uint individual,uint gene)
{
    uint8_t randval = rand()%8;
    //uint8_t randpos = rand()%30;
    if(randval == 0)//Farbe mutieren
    {
        m_individuals[individual].gene[gene].colr += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colg += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colb += (rand()%(mutsize*2))-mutsize;
        return;
    }else if(randval == 7)
        change_position(individual,gene,rand()%m_genomesize);
    #define m_ind1 m_individuals[individual].gene[gene]
    int wert = randval == 1 ? m_ind1.x1 : randval == 2 ? m_ind1.y1 : randval == 3 ? m_ind1.x2 : randval == 4 ? m_ind1.y2 : randval == 5 ? m_ind1.x3 : m_ind1.y3;
    if(wert < (int)mutsize)
        (randval == 1 ? m_ind1.x1 : randval == 2 ? m_ind1.y1 : randval == 3 ? m_ind1.x2 : randval == 4 ? m_ind1.y2 : randval == 5 ? m_ind1.x3 : m_ind1.y3) += rand()%(mutsize*2);
    else
        (randval == 1 ? m_ind1.x1 : randval == 2 ? m_ind1.y1 : randval == 3 ? m_ind1.x2 : randval == 4 ? m_ind1.y2 : randval == 5 ? m_ind1.x3 : m_ind1.y3) += rand()%(mutsize*2)-mutsize;
}

template<>
void GeneticAlgorithm<EllipseData>::mutate_single(uint individual,uint gene)
{
    uint8_t randval = rand()%6;
    if(randval == 0)//Farbe mutieren
    {
        m_individuals[individual].gene[gene].colr += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colg += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colb += (rand()%(mutsize*2))-mutsize;
        return;
    }
    if(randval == 1)
    {
        if(m_individuals[individual].gene[gene].midx < mutsize)
            m_individuals[individual].gene[gene].midx += rand()%(mutsize*2);
        else
            m_individuals[individual].gene[gene].midx += (rand()%(mutsize*2))-mutsize;
    }else if(randval == 2)
    {
        if(m_individuals[individual].gene[gene].midy < mutsize)
            m_individuals[individual].gene[gene].midy += rand()%(mutsize*2);
        else
            m_individuals[individual].gene[gene].midy += (rand()%(mutsize*2))-mutsize;
    }else if(randval == 3)
    {
        if(m_individuals[individual].gene[gene].radiusx < (mutsize/4))
            m_individuals[individual].gene[gene].radiusx += rand()%(mutsize/2);
        else
            m_individuals[individual].gene[gene].radiusx += (rand()%(mutsize/2))-(mutsize/4);
    }else if(randval == 4)
    {
        if(m_individuals[individual].gene[gene].radiusy < (mutsize/4))
            m_individuals[individual].gene[gene].radiusy += rand()%(mutsize/2);
        else
            m_individuals[individual].gene[gene].radiusy += (rand()%(mutsize/2))-(mutsize/4);
    }else
        change_position(individual,gene,rand()%m_genomesize);
}

template<>
void GeneticAlgorithm<CircleData>::mutate_single(uint individual,uint gene)
{
    uint8_t randval = rand()%5;
    if(randval == 0)//Farbe mutieren
    {
        m_individuals[individual].gene[gene].colr += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colg += (rand()%(mutsize*2))-mutsize;
        m_individuals[individual].gene[gene].colb += (rand()%(mutsize*2))-mutsize;
        return;
    }
    if(randval == 1)
    {
        if(m_individuals[individual].gene[gene].midx < mutsize)
            m_individuals[individual].gene[gene].midx += rand()%(mutsize*2);
        else
            m_individuals[individual].gene[gene].midx += (rand()%(mutsize*2))-mutsize;
    }else if(randval == 2)
    {
        if(m_individuals[individual].gene[gene].midy < mutsize)
            m_individuals[individual].gene[gene].midy += rand()%(mutsize*2);
        else
            m_individuals[individual].gene[gene].midy += (rand()%(mutsize*2))-mutsize;
    }else if(randval == 3)
    {
        if(m_individuals[individual].gene[gene].radius < (mutsize/4))
            m_individuals[individual].gene[gene].radius += rand()%(mutsize/2);
        else
            m_individuals[individual].gene[gene].radius += (rand()%(mutsize/2))-(mutsize/4);
    }else
        change_position(individual,gene,rand()%m_genomesize);
}

template<class T>
void GeneticAlgorithm<T>::random_assign(uint,int)
{
    cout <<"Dieser Datentyp wird nicht unterstützt!"<<endl;
}
/* Die Variable Spawn legt fest, wo im Bildbereich die Formen am Anfang ersheinen sollen:
 * 0: einfach zufällig verteilt
 * 1: Alle in der Mitte
*/
template<>
void GeneticAlgorithm<TriangleData>::random_assign(uint individual,int spawn)
{
    for(uint i = 0;i<m_genomesize;i++)
    {
        m_individuals[individual].gene[i].x1 = spawn ? picsizex/2-rand()%(startsize/2) : rand()%picsizex;
        m_individuals[individual].gene[i].y1 = spawn ? picsizey/2-rand()%(startsize/2) : rand()%picsizey;
        m_individuals[individual].gene[i].x2 = m_individuals[individual].gene[i].x1+rand()%startsize;
        m_individuals[individual].gene[i].y2 = m_individuals[individual].gene[i].y1+rand()%startsize;
        m_individuals[individual].gene[i].x3 = m_individuals[individual].gene[i].x1+rand()%startsize;
        m_individuals[individual].gene[i].y3 = m_individuals[individual].gene[i].y1+rand()%startsize;

        m_individuals[individual].gene[i].colr = rand()%256;
        m_individuals[individual].gene[i].colg = rand()%256;
        m_individuals[individual].gene[i].colb = rand()%256;
    }
}

template<>
void GeneticAlgorithm<EllipseData>::random_assign(uint individual,int spawn)
{
    for(uint i = 0;i<m_genomesize;i++)
    {
        m_individuals[individual].gene[i].midx = spawn ? picsizex/2 : rand()%picsizex;
        m_individuals[individual].gene[i].midy = spawn ? picsizey/2: rand()%picsizey;
        m_individuals[individual].gene[i].radiusx = rand()%startsize;
        m_individuals[individual].gene[i].radiusy = rand()%startsize;

        m_individuals[individual].gene[i].colr = rand()%256;
        m_individuals[individual].gene[i].colg = rand()%256;
        m_individuals[individual].gene[i].colb = rand()%256;
    }
}

template<>
void GeneticAlgorithm<CircleData>::random_assign(uint individual,int spawn)
{
    for(uint i = 0;i<m_genomesize;i++)
    {
        m_individuals[individual].gene[i].midx = spawn ? picsizex/2 : rand()%picsizex;
        m_individuals[individual].gene[i].midy = spawn ? picsizey/2: rand()%picsizey;
        m_individuals[individual].gene[i].radius = rand()%startsize;

        m_individuals[individual].gene[i].colr = rand()%256;
        m_individuals[individual].gene[i].colg = rand()%256;
        m_individuals[individual].gene[i].colb = rand()%256;
    }
}

template<class T>
void GeneticAlgorithm<T>::paint_best(QPainter * painter)
{
    bestpic_action.lock();
    for(uint x = 0;x<picsizex;x++)
    {
        for(uint y = 0;y<picsizey;y++)
        {
            QPen b = painter->pen();
            //cout <<"Bild: "<<render_pic[x][y].r<<" "<<render_pic[x][y].g<<" "<<render_pic[x][y].b<<endl;
            QColor col(best_pic[x][y].r,best_pic[x][y].g,best_pic[x][y].b);
            //QColor col(pic_compare[x][y].r,pic_compare[x][y].g,pic_compare[x][y].b);
            b.setColor(col);
            painter->setPen(b);
            painter->drawPoint(x,y);
        }
    }
    bestpic_action.unlock();
}

template<class T>
void GeneticAlgorithm<T>::paint_best(QImage * image)
{
    bestpic_action.lock();
    QRgb value;
    for(uint x = 0;x<picsizex;x++)
    {
        for(uint y = 0;y<picsizey;y++)
        {

            value = qRgb(best_pic[x][y].r,best_pic[x][y].g,best_pic[x][y].b);
            image->setPixel(x,y,value);
        }
    }
    bestpic_action.unlock();
}

template<class T>
void GeneticAlgorithm<T>::save_evolution(QString filename)
{
    ofstream file(filename.toStdString().c_str(), ios::app | ios::binary);
    //EVO: sichert, dass es wirklich eine Evolutionsdatei ist
    //1: Versionsnummer
    file <<"EVO"<<1<<" ";
    file <<m_populationsize<<" "<<m_genomesize<<" "<<picsizex<<" "<<picsizey<<" ";
    file <<blackwhite<<" "<<startsize<<" ";

    file.close();
    load_evolution(filename);
}

template<class T>
void GeneticAlgorithm<T>::load_evolution(QString filename)
{
    /*ifstream file(filename.toStdString().c_str(),ios::binary);
    file >>"EVO">>1>>" ";
    file >>m_populationsize>>" ">>m_genomesize>>" ">>picsizex>>" ">>picsizey>>" ";
    cout <<"Daten: "<<m_populationsize<<" "<<m_genomesize<<" "<<picsizex<<" "<<picsizey<<" ";
    file >>blackwhite>>" ">>startsize>>" ";
    file.close();*/
}

struct comb
{
    uint64_t fit;
    uint num;
};
bool isBigger(comb c1,comb c2){return c1.fit<c2.fit;}

template<class T>
void GeneticAlgorithm<T>::working_thread()
{
    //cout <<"Anzahl: "<<m_populationsize<<endl;
    Farbe ** buffer = new Farbe*[picsizex];
    for(uint i = 0;i<picsizex;i++)
        buffer[i] = new Farbe[picsizey];
    while(1)
    {
        number_action.lock();
        if(actnum == m_populationsize)
        {
            for(uint i = 0;i<picsizex;i++)
                delete [] buffer[i];
            delete [] buffer;
            number_action.unlock();
            ready--;
            return;
        }
        uint arbeit = actnum;
        actnum++;
        number_action.unlock();
        fitn[arbeit] = fitness(arbeit,buffer,fitnessmode);
        //cout <<"Fitness von "<<arbeit<<" ist "<<fitn[arbeit]<<endl;
    }
}

template<class T>
int GeneticAlgorithm<T>::generation()
{
    //Falls die Einstellungen geändert wurden: neu laden
    varbuffer_action.lock();
    if(varbuffer_changed)
    {
        cout <<"Variablen neu laden!"<<endl;
        threadnum = varbuffer[0];
        fitnessmode = varbuffer[1];
        mutsize = varbuffer[2];
        mutrate = varbuffer[3];
        xovertype = varbuffer[4];
        varbuffer_changed = false;
    }
    varbuffer_action.unlock();

    #define rankspaceconst 0.3
    mutate_all();
    actnum = 0;
    ready = threadnum;
    /*QElapsedTimer timer;
    timer.start();*/
    QThread ** threads;
    genetic_threader **  threaders;
    if(threadnum > 1)
    {
        threads = new QThread*[threadnum-1];
        threaders = new genetic_threader*[threadnum-1];
    }
    for(uint i = 0;i<threadnum-1;i++)
    {
        threads[i] = new QThread;
        threaders[i] = new genetic_threader(this);
        threaders[i]->moveToThread(threads[i]);
        //connect(threader, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(threads[i], SIGNAL(started()), threaders[i], SLOT(process()));
        connect(threaders[i], SIGNAL(finished()), threads[i], SLOT(deleteLater()));
        connect(threaders[i], SIGNAL(finished()), threaders[i], SLOT(deleteLater()));
        connect(threads[i], SIGNAL(finished()), threads[i], SLOT(deleteLater()));
        threads[i]->start();
    }
    working_thread();
    while(ready);
    for(uint i = 0;i<threadnum-1;i++)
        threads[i]->quit();
    vector<comb> fit;
    for(uint i = 0;i<m_populationsize;i++)
        fit.push_back((comb){fitn[i],i});
    sort(fit.begin(), fit.end(), isBigger);
    //cout << "Sortieren: " << timer.elapsed() << " ms"<<endl;
    uint64_t ret = fitness(fit[0].num,render_pic,fitnessmode);
    bestpic_to_act();
    vector<individual> result;
    for(uint i = 0;i<m_populationsize/3;i++)
        result.push_back(m_individuals[fit[i].num]);
    for(uint i = m_populationsize/3;i<m_populationsize;i++)
    {
        float prob = pow(1-rankspaceconst,i-m_populationsize/3)*rankspaceconst;
        if(double(rand())/double(RAND_MAX) < prob)
        {
            result.push_back(m_individuals[fit[i].num]);
        }
    }
    for(uint i = 0;i<result.size();i++)
    {
        m_individuals[i] = result[i];
    }
    uint counter = result.size();
    const uint startcount = result.size();
    while(counter < m_populationsize)
    {
        int start = 0,cross = 1;
        while(rand()%20 > 14)
            start++;
        while(rand()%20 > 14 || start == cross)
            cross++;
        if(start >= startcount)
            start = 0;
        if(cross >= startcount)
            cross = 1;
        if(xovertype == 0)//Standardcrossover
        {
            uint border = rand()%m_genomesize;
            for(uint i = 0;i<m_genomesize;i++)
            {
                if(i < border)
                {
                    m_individuals[counter].gene[i] = result[start].gene[i];
                    m_individuals[counter+1].gene[i] = result[cross].gene[i];
                }else
                {
                    m_individuals[counter+1].gene[i] = result[start].gene[i];
                    m_individuals[counter].gene[i] = result[cross].gene[i];
                }
            }
        }else if(xovertype == 1)//uniform crossover
        {
            for(uint i = 0;i<m_genomesize;i++)
            {
                if(rand()%2 == 0)
                {
                    m_individuals[counter].gene[i] = result[start].gene[i];
                    m_individuals[counter+1].gene[i] = result[cross].gene[i];
                }else
                {
                    m_individuals[counter+1].gene[i] = result[start].gene[i];
                    m_individuals[counter].gene[i] = result[cross].gene[i];
                }
            }
        }else if(xovertype == 3)
        {
            for(uint i = 0;i<m_genomesize;i++)
            {
                m_individuals[counter].gene[i] = result[start].gene[i];
                m_individuals[counter+1].gene[i] = result[cross].gene[i];
            }
        }
        counter += 2;
    }
    fit.clear();
    result.clear();

    /*if(threadnum > 1)
    {
        for(uint i = 0;i<threadnum-1;i++)
        {
            //threads[i]->terminate();
        }
        for(uint i = 0;i<threadnum-1;i++){
            //delete threads[i];
            delete threaders[i];
        }
        //delete [] threads;
        delete [] threaders;
    }*/

    //cout << "Rest: " << timer.elapsed() << " ms"<<endl;
    return ret;
}

template<class T>
uint64_t GeneticAlgorithm<T>::fitness(uint individual,Farbe ** renderbuffer,uint fitmode)
{
    for(uint x = 0;x<picsizex;x++)
        for(uint y = 0;y<picsizey;y++)
        {
            renderbuffer[x][y].r = 255;
            renderbuffer[x][y].g = 255;
            renderbuffer[x][y].b = 255;
        }
    for(uint i = 0;i<m_genomesize;i++)
    {
        paint(individual,i,renderbuffer);
    }
    uint64_t error = 0;
    for(uint x = 0;x<picsizex;x++)
    {
        for(uint y = 0;y<picsizey;y++)
        {
            if(blackwhite)
            {
                error += pow(abs(pic_compare[x][y].r-renderbuffer[x][y].r),fitmode+1);
            }else
            {
                error += pow(abs(pic_compare[x][y].r-renderbuffer[x][y].r),fitmode+1);
                error += pow(abs(pic_compare[x][y].g-renderbuffer[x][y].g),fitmode+1);
                error += pow(abs(pic_compare[x][y].b-renderbuffer[x][y].b),fitmode+1);
            }
        }
    }
    return error;
}

template<class T>
void GeneticAlgorithm<T>::paint(uint,uint,Farbe **)
{
    cout <<"Dieser Datentyp kann nicht gezeichnet werden!"<<endl;
}

template<class T>
void GeneticAlgorithm<T>::row(uint x,uint y,uint width,Farbe col,Farbe ** renderbuffer)
{
    if(y < 0 || y >= picsizey)
        return;
    for(uint i = (x<0?0:x); i<width+x;i++)
    {
        if(i >= picsizex)
            return;
        renderbuffer[i][y] = col;
    }
}

template<>
void GeneticAlgorithm<EllipseData>::paint(uint individual,uint gene,Farbe ** renderbuffer)
{
    #define incx() x++, dxt += d2xt, t += dxt
    #define incy() y--, dyt += d2yt, t += dyt
    int xc = m_individuals[individual].gene[gene].midx,yc = m_individuals[individual].gene[gene].midy;
    //cout <<"Ellipsenposition: "<<xc<<" "<<yc<<endl;
    int a = m_individuals[individual].gene[gene].radiusx,b = m_individuals[individual].gene[gene].radiusy;
    Farbe col = (Farbe){m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr};
    if(!blackwhite)
    {
        col.b = m_individuals[individual].gene[gene].colb;
        col.g = m_individuals[individual].gene[gene].colg;
    }
    int x = 0, y = b;
    unsigned int width = 1;
    long a2 = (long)a*a, b2 = (long)b*b;
    long crit1 = -(a2/4 + a%2 + b2);
    long crit2 = -(b2/4 + b%2 + a2);
    long crit3 = -(b2/4 + b%2);
    long t = -a2*y; /* e(x+1/2,y-1/2) - (a^2+b^2)/4 */
    long dxt = 2*b2*x, dyt = -2*a2*y;
    long d2xt = 2*b2, d2yt = 2*a2;

    while (y>=0 && x<=a) {
        if (t + b2*x <= crit1 ||     /* e(x+1,y-1/2) <= 0 */
            t + a2*y <= crit3) {     /* e(x+1/2,y) <= 0 */
            incx();
            width += 2;
        }
        else if (t - a2*y > crit2) { /* e(x+1/2,y-1) > 0 */
            row(xc-x, yc-y, width,col,renderbuffer);
            if (y!=0)
                row(xc-x, yc+y, width,col,renderbuffer);
            incy();
        }
        else {
            row(xc-x, yc-y, width,col,renderbuffer);
            if (y!=0)
                row(xc-x, yc+y, width,col,renderbuffer);
            incx();
            incy();
            width += 2;
        }
    }
    if (b == 0)
        row(xc-a, yc, 2*a+1,col,renderbuffer);
}

template<>
void GeneticAlgorithm<CircleData>::paint(uint individual,uint gene,Farbe ** renderbuffer)
{
    int x0 = m_individuals[individual].gene[gene].midx,y0 = m_individuals[individual].gene[gene].midy;
    int radius = m_individuals[individual].gene[gene].radius;
    Farbe col = (Farbe){m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr};
    if(!blackwhite)
    {
        col.b = m_individuals[individual].gene[gene].colb;
        col.g = m_individuals[individual].gene[gene].colg;
    }
    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = x0 - x; i <= x0 + x; i++)
        {
            if(i > 0 && y0+y > 0 && i < int(picsizex) && y0+y < int(picsizey))renderbuffer[i][y0+y] = col;
            if(i > 0 && y0-y > 0 && i < int(picsizex) && y0-y < int(picsizey))renderbuffer[i][y0-y] = col;
        }
        for (int i = x0 - y; i <= x0 + y; i++)
        {
            if(i > 0 && y0+x > 0 && i < int(picsizex) && y0+x < int(picsizey))renderbuffer[i][y0+x] = col;
            if(i > 0 && y0-x > 0 && i < int(picsizex) && y0-x < int(picsizey))renderbuffer[i][y0-x] = col;
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

template<class T>
void GeneticAlgorithm<T>::SWAP(int& a, int& b)//Vertauscht zwei interger, ist für triangle benötigt
{
      int temp = a;
      a = b;
      b = temp;
};

template<>
void GeneticAlgorithm<TriangleData>::paint(uint individual,uint gene,Farbe ** renderbuffer)
{
    int x0 = m_individuals[individual].gene[gene].x1,y0 = m_individuals[individual].gene[gene].y1;
    int x1 = m_individuals[individual].gene[gene].x2,y1 = m_individuals[individual].gene[gene].y2;
    int x2 = m_individuals[individual].gene[gene].x3,y2 = m_individuals[individual].gene[gene].y3;
    Farbe col = (Farbe){m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr,m_individuals[individual].gene[gene].colr};
    if(!blackwhite)
    {
        col.b = m_individuals[individual].gene[gene].colb;
        col.g = m_individuals[individual].gene[gene].colg;
    }
    if( y1 < y0 )//Vertauscht alle Werte so, dass sie geordnet sind
    {
        SWAP(y1, y0);
        SWAP(x1, x0);
    }
    if( y2 < y0 )
    {
        SWAP(y2, y0);
        SWAP(x2, x0);
    }
    if( y1 < y2 )
    {
        SWAP(y2, y1);
        SWAP(x2, x1);
    }
    float xl_edge = (float)x0;  // left edge
    float xr_edge = (float)x0;  // right edge
    float dxldy;
    float dxrdy;
    float dxdy1 = (float)(x2-x0)/(y2-y0);//Die x-y Verhältnisse
    float dxdy2 = (float)(x1-x0)/(y1-y0);
    if( dxdy1 < dxdy2 )
    {
        dxldy = dxdy1;
        dxrdy = dxdy2;
    }else{
        dxldy = dxdy2;
        dxrdy = dxdy1;
    }
    for(int y=y0; y<y2; y++)//Einen Teil zeichnen
    {
        if(y >= 0 && y < int(picsizey) && xl_edge >= 0)
        {
            for(int x=xl_edge; x<xr_edge; x++)
            {
                if(x >= int(picsizex))
                    break;
                renderbuffer[x][y] = col;
            }
        }
        xl_edge = xl_edge + dxldy;
        xr_edge = xr_edge + dxrdy;
    }
    if( dxdy1 < dxdy2 )
    {
        dxldy = (float)(x2-x1)/(y2-y1);
        xl_edge = x2;//Um die untere Hälfte zeichnen zu können, falls es keine obere gibt
    }else{
        dxrdy = (float)(x2-x1)/(y2-y1);
        xr_edge = x2;
    }
    for(int y=y2; y<y1; y++)//Den zweiten Teil zeichnen
    {
        if(y >= 0 && y < int(picsizey) && xl_edge >= 0)
        {
            for(int x=xl_edge; x<xr_edge; x++)
            {
                if(x >= int(picsizex))
                    break;
                renderbuffer[x][y] = col;
            }
        }
        xl_edge = xl_edge + dxldy;
        xr_edge = xr_edge + dxrdy;
    }
}

template class GeneticAlgorithm<EllipseData>;
template class GeneticAlgorithm<CircleData>;
template class GeneticAlgorithm<TriangleData>;
